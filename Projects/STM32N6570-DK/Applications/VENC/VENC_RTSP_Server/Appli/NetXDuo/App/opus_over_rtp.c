/**
  ******************************************************************************
  * @file           : opus_over_rtp.c
  * @brief          : OPUS over RTP application (C source)
  *******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *******************************************************************************
  */
#include "nx_rtp_sender.h"
#include "opus.h"
#include "time_monitor.h"
#include "stm32n6xx_hal.h"
#include "plugin_audio.h"

UINT _nx_rtp_sender_session_packet_allocate(NX_RTP_SESSION *session, NX_PACKET **packet_ptr, ULONG wait_option);
UINT _nx_rtp_sender_session_packet_send(NX_RTP_SESSION *session, NX_PACKET *packet_ptr, ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker);

UINT _nxe_rtp_sender_session_opus_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                      ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker);
UINT _nx_rtp_sender_session_opus_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                     ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker);


UINT nx_rtp_sender_session_audio_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                        ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker)
{
  /* Forward Opus audio frames to the NetX RTP helper. */
  return  _nxe_rtp_sender_session_opus_send(session , frame_data, frame_data_size,
                                             timestamp, ntp_msw, ntp_lsw, marker);
}       
static CHAR *sdp =
#if 1
/* SDP string options */
"v=0\r\ns=H264 video with OPUS audio, streamed by the NetX RTSP Server\r\n\
m=video 0 RTP/AVP 96\r\n\
a=rtpmap:96 H264/90000\r\n\
a=fmtp:96 profile-level-id=42A01E; packetization-mode=1\r\n\
a=control:trackID=0\r\n\
m=audio 0 RTP/AVP 97\r\n\
a=rtpmap:97  opus/16000/1\r\n\
a=control:trackID=1\r\n";
#endif

#if 0
"v=0\r\ns=Debug audio, streamed by the NetX RTSP Server\r\n\
m=audio 0 RTP/AVP 97\r\n\
a=rtpmap:97 opus/16000/1\r\n\
a=control:trackID=1\r\n";
#endif


UCHAR * nx_rtsp_server_get_sdp(void)
{
  return (UCHAR *)sdp;
}


ULONG nx_rtsp_server_get_audio_payload_type(void)
{
  return 97;
}


/* ==================================================================================================*/
/* opus AddOn */
/* ==================================================================================================*/
UINT _nx_rtp_sender_session_opus_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                     ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker);

   
 /* Debug and monitoring  functions*/

__weak void dump_audio_samples(const char *filename, const uint8_t *samples, size_t sample_count)
{
  return;
}

__weak void override_audio_input(int16_t *samples, size_t sample_count)
{
  return;
}

__weak void timeMonitorStart(void)
{
  return;
}

__weak void timeMonitorStop(void)
{
  return;
}

__weak void monitor_bitrate(const char* stream, uint32_t frameSize)
{
  return;
}


//OpusEncoder * opus_encoder = NULL;
//#define NB_SAMPLES_PER_FRAME  (20*16) /* 20ms @ 16KHz */
//#define NB_BYTES_PER_FRAME    (NB_SAMPLES_PER_FRAME*2) /* 16bpsample*/
//
//int32_t opus_encode_ext(uint8_t *frame_data, int16_t frame_data_size, uint8_t **data) {
//    int nb_encoded_bytes = 0;
//    static uint32_t  last_audio_frame_number = 0;
//    uint32_t  new_audio_frame_number = 0;
//    int16_t frame_data_nb_samples = frame_data_size / sizeof(int16_t);
//    static uint8_t encoded_data[NB_BYTES_PER_FRAME];
//
//    if (opus_encoder == NULL) {
//        opus_encoder = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &nb_encoded_bytes);
//        if (opus_encoder == NULL || nb_encoded_bytes != OPUS_OK) {
//            // Handle error appropriately, e.g., log or return specific error code
//            return -1;
//        }
//    }
//
//    if (frame_data_size != NB_BYTES_PER_FRAME) {
//      printf("Invalid frame size: %d\n", frame_data_size);
//      return -1;
//  }
//
//    *data = encoded_data;
//    
//    /* Debug : override input audio*/
//    override_audio_input((int16_t *)frame_data, frame_data_nb_samples);
//   
//    /* Debug : monitor encode time*/
//    timeMonitorStart();
//    
//     /* Converts frame size (bytes) => Frame size(nb samples)*/
//    nb_encoded_bytes =  opus_encode(opus_encoder, (const opus_int16 *)frame_data, frame_data_nb_samples, *data, NB_BYTES_PER_FRAME);
//    
//    /* Debug : monitor encode time*/
//    timeMonitorStop();
//   
//    new_audio_frame_number =  AUDIO_APP_LastPcmFrameNumber(); 
//    
//    /* Check overflow*/
//    if (last_audio_frame_number + 1 != new_audio_frame_number)
//    {
//      printf("Audio Overflow: %d\n", frame_data_size);
//    }
//    last_audio_frame_number = new_audio_frame_number;
//        
//    
//    return nb_encoded_bytes;
//}


UINT _nxe_rtp_sender_session_opus_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                       ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker)
{
  
  int32_t status;
    
  /* Check for invalid input pointers. */
  if ((session == NX_NULL) || (session -> nx_rtp_sender == NX_NULL) || (session -> nx_rtp_session_id != NX_RTP_SESSION_ID) || (frame_data == NX_NULL))
  {
    return(NX_PTR_ERROR);
  }
  
  /* If valid, status = number of encoded bytes */
  /* Call actual RTP sender session frame send service. */
  status = _nx_rtp_sender_session_opus_send(session, frame_data, frame_data_size, timestamp, ntp_msw, ntp_lsw, marker);
  
  /* Return status. */
  return(status);
}


UINT _nx_rtp_sender_session_opus_send(NX_RTP_SESSION *session, UCHAR *frame_data, ULONG frame_data_size,
                                     ULONG timestamp, ULONG ntp_msw, ULONG ntp_lsw, UINT marker)
{
  
  UINT       status;
  ULONG      send_packet_length;
  ULONG      max_packet_length = session -> nx_rtp_session_max_packet_size;
  NX_PACKET *send_packet = NX_NULL;
  UCHAR     *data_ptr;
  UINT       temp_marker;
  
  
  /* In current design, the marker bit shall be always 1 (i.e. a complete adpcm frame required to be passed). */
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
    
    /* Send adpcm frame through rtp; passed marker bit with true when this is the last frame packet. */
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

