/**
  ******************************************************************************
  * @file           : pcm_over_rtp.c
  * @brief          : PCM over RTP application (C source)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include    "nx_rtp_sender.h"
#include "stm32n6xx_hal.h"

UINT _nx_rtp_sender_session_packet_allocate(NX_RTP_SESSION *session, NX_PACKET **packet_ptr, ULONG wait_option);
UINT _nx_rtp_sender_session_packet_send(NX_RTP_SESSION *session, NX_PACKET *packet_ptr, ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker);

UINT _nxe_rtp_sender_session_pcm_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                      ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker);
UINT _nx_rtp_sender_session_pcm_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                     ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker);


UINT nx_rtp_sender_session_audio_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                        ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker)
{
  /* Forward PCM audio frames to the NetX RTP helper. */
  return  _nxe_rtp_sender_session_pcm_send(session , frame_data, frame_data_size,
                                             timestamp, ntp_msw, ntp_lsw, marker);
}       

/* SDP string options */
static CHAR *sdp =
#if 1 /* h264 + PCM */
"v=0\r\ns=H264 video with PCM audio, streamed by the NetX RTSP Server\r\n\
m=video 0 RTP/AVP 96\r\n\
a=rtpmap:96 H264/90000\r\n\
a=fmtp:96 profile-level-id=42A01E; packetization-mode=1\r\n\
a=control:trackID=0\r\n\
m=audio 0 RTP/AVP 11\r\n\
a=rtpmap:11 L16/16000/1\r\n\
a=fmtp:11 endian=big\r\n\
a=control:trackID=1\r\n";
#endif 

#if 0/* PCM Only */
"v=0\r\ns=PCM audio, streamed by the NetX RTSP Server\r\n\
m=audio 0 RTP/AVP 11\r\n\
a=rtpmap:11 L16/16000/1\r\n\
a=fmtp:11 endian=big\r\n\
a=control:trackID=1\r\n";
#endif


UCHAR * nx_rtsp_server_get_sdp(void)
{
  return (UCHAR *)sdp;
}

ULONG nx_rtsp_server_get_audio_payload_type(void)
{
  return 11;
}


__weak void override_audio_input(INT *samples, size_t sample_count) 

{
  return;
}

__weak void monitor_bitrate(const char* stream, uint32_t frameSize)
{
  return;
}

/* ==================================================================================================*/
/* PCM AddOn */
/* ==================================================================================================*/
UINT _nx_rtp_sender_session_pcm_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                     ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker);

UINT _nxe_rtp_sender_session_pcm_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                      ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker)
{
  
  UINT status;
  INT frame_data_nb_samples = frame_data_size / sizeof(INT);
 
  
  /* Check for invalid input pointers. */
  if ((session == NX_NULL) || (session -> nx_rtp_sender == NX_NULL) || (session -> nx_rtp_session_id != NX_RTP_SESSION_ID) || (frame_data == NX_NULL))
  {
    return(NX_PTR_ERROR);
  }
  
  
  /* Ensure that frame_data_size is a multiple of 2, as we are dealing with 16-bit samples. */
  if (frame_data_size % 2 != 0)
  {
    return(NX_SIZE_ERROR);
  }

    /* Debug : override input audio*/
    override_audio_input((INT *)frame_data, frame_data_nb_samples);

  
  /* Change endianness of the frame data. */
  /* This is because client seems to expect big endian whearas we capture little endian*/
  for (ULONG i = 0; i < frame_data_size; i += 2)
  {
    UCHAR temp = frame_data[i];
    frame_data[i] = frame_data[i + 1];
    frame_data[i + 1] = temp;
  }
  
  /* Call actual RTP sender session frame send service. */
  status = _nx_rtp_sender_session_pcm_send(session, frame_data, frame_data_size, timestamp, ntp_msw, ntp_lsw, marker);
  
  /* Return status. */
  return(status);
}


UINT _nx_rtp_sender_session_pcm_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                     ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker)
{
  
  UINT       status;
  ULONG      send_packet_length;
  ULONG      max_packet_length = session -> nx_rtp_session_max_packet_size;
  NX_PACKET *send_packet = NX_NULL;
  UCHAR     *data_ptr;
  UINT       temp_marker;
  
  
  /* In current design, the marker bit shall be always 1 (i.e. a complete pcm frame required to be passed). */
  if (marker != NX_TRUE)
  {
    return(NX_NOT_SUPPORTED);
  }
  
  /* Initialize data_ptr to where data bytes begin. */
  data_ptr = frame_data;

  monitor_bitrate("audio",frame_data_size);
  
  while (frame_data_size > 0)
  {
    
    /* Allocate a rtp packet. */
    status = _nx_rtp_sender_session_packet_allocate(session, &send_packet, NX_RTP_SENDER_PACKET_TIMEOUT);
    if (status)
    {
      return(status);
    }
    
    /* Check if fragmentation needed, and assign data length. */
    if (frame_data_size > max_packet_length)
    {
      send_packet_length = max_packet_length;
      temp_marker = NX_FALSE;
    }
    else
    {
      send_packet_length = frame_data_size;
      temp_marker = NX_TRUE;
    }
    
    /* Copy payload data into the packet. */
    status = nx_packet_data_append(send_packet, (void *)data_ptr, send_packet_length,
                                   session -> nx_rtp_sender -> nx_rtp_sender_packet_pool_ptr, NX_RTP_SENDER_PACKET_TIMEOUT);
    if (status)
    {
      nx_packet_release(send_packet);
      return(status);
    }
    
    /* Send PCM frame through rtp; passed marker bit with true when this is the last frame packet. */
    status = _nx_rtp_sender_session_packet_send(session, send_packet, timestamp, ntp_msw, ntp_lsw, temp_marker);
    if (status)
    {
      nx_packet_release(send_packet);
      return(status);
    }
    
    /* Compute remaining frame length and move data pointer. */
    frame_data_size -= send_packet_length;
    data_ptr += send_packet_length;
  }
  
  /* Return success status. */
  return(NX_SUCCESS);
}

