/*****************************************************************
 * lqtgavl - gavl Bindings for libquicktime
 *
 * Copyright (c) 2001 - 2008 Members of the Gmerlin project
 * gmerlin-general@lists.sourceforge.net
 * http://gmerlin.sourceforge.net
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * *****************************************************************/

#include <stdlib.h>
#include <lqtgavl.h>
#include <colormodels.h>

/* Sampleformat conversion */

static const struct
  {
  lqt_sample_format_t lqt;
  gavl_sample_format_t gavl;
  }
sampleformats[] =
  {
    { LQT_SAMPLE_INT8,  GAVL_SAMPLE_S8 },
    { LQT_SAMPLE_UINT8, GAVL_SAMPLE_U8 },
    { LQT_SAMPLE_INT16, GAVL_SAMPLE_S16 },
    { LQT_SAMPLE_INT32, GAVL_SAMPLE_S32 },
    { LQT_SAMPLE_FLOAT, GAVL_SAMPLE_FLOAT }//,
 /*   { LQT_SAMPLE_DOUBLE, GAVL_SAMPLE_DOUBLE } */ // Ubuntu Hardy ships with libquicktime-to-old
  };

static gavl_sample_format_t
sampleformat_lqt_2_gavl(lqt_sample_format_t format)
  {
  int i;
  for(i = 0; i < sizeof(sampleformats) / sizeof(sampleformats[0]); i++)
    {
    if(sampleformats[i].lqt == format)
      return sampleformats[i].gavl;
    }
  return GAVL_SAMPLE_NONE;
  }

/* Chroma placement */

static const struct
  {
  lqt_chroma_placement_t lqt;
  gavl_chroma_placement_t gavl;
  }
chroma_placements[] =
  {
    {  LQT_CHROMA_PLACEMENT_DEFAULT, GAVL_CHROMA_PLACEMENT_DEFAULT },
    {  LQT_CHROMA_PLACEMENT_MPEG2,   GAVL_CHROMA_PLACEMENT_MPEG2   },
    {  LQT_CHROMA_PLACEMENT_DVPAL,   GAVL_CHROMA_PLACEMENT_DVPAL   },
  };

static gavl_chroma_placement_t
chroma_placement_lqt_2_gavl(lqt_chroma_placement_t chroma_placement)
  {
  int i;
  for(i = 0; i < sizeof(chroma_placements) / sizeof(chroma_placements[0]); i++)
    {
    if(chroma_placements[i].lqt == chroma_placement)
      return chroma_placements[i].gavl;
    }
  return GAVL_CHROMA_PLACEMENT_DEFAULT;
  }

/* Interlace mode */

static const struct
  {
  lqt_interlace_mode_t lqt;
  gavl_interlace_mode_t gavl;
  }
interlace_modes[] =
  {
    { LQT_INTERLACE_NONE,         GAVL_INTERLACE_NONE},
    { LQT_INTERLACE_TOP_FIRST,    GAVL_INTERLACE_TOP_FIRST },
    { LQT_INTERLACE_BOTTOM_FIRST, GAVL_INTERLACE_BOTTOM_FIRST },
  };

static gavl_interlace_mode_t
interlace_mode_lqt_2_gavl(lqt_interlace_mode_t interlace_mode)
  {
  int i;
  for(i = 0; i < sizeof(interlace_modes) / sizeof(interlace_modes[0]); i++)
    {
    if(interlace_modes[i].lqt == interlace_mode)
      return interlace_modes[i].gavl;
    }
  return GAVL_INTERLACE_NONE;
  }

static lqt_interlace_mode_t
interlace_mode_gavl_2_lqt(gavl_interlace_mode_t interlace_mode)
  {
  int i;
  for(i = 0; i < sizeof(interlace_modes) / sizeof(interlace_modes[0]); i++)
    {
    if(interlace_modes[i].gavl == interlace_mode)
      return interlace_modes[i].lqt;
    }
  return LQT_INTERLACE_NONE;
  }

/* Pixelformats */

static const struct
  {
  int lqt;
  gavl_pixelformat_t gavl;
  }
pixelformats[] =
  {
    { BC_RGB565,       GAVL_RGB_16 },
    { BC_BGR565,       GAVL_BGR_16 },
    { BC_BGR888,       GAVL_BGR_24 },
    { BC_BGR8888,      GAVL_BGR_32 },
    { BC_RGB888,       GAVL_RGB_24 },
    { BC_RGBA8888,     GAVL_RGBA_32 },
    { BC_RGB161616,    GAVL_RGB_48 },
    { BC_RGBA16161616, GAVL_RGBA_64 },
    { BC_YUVA8888,     GAVL_YUVA_32 },
    { BC_YUV422,       GAVL_YUY2 },
    { BC_YUV420P,      GAVL_YUV_420_P },
    { BC_YUV422P,      GAVL_YUV_422_P },
    { BC_YUV444P,      GAVL_YUV_444_P },
    { BC_YUVJ420P,     GAVL_YUVJ_420_P },
    { BC_YUVJ422P,     GAVL_YUVJ_422_P },
    { BC_YUVJ444P,     GAVL_YUVJ_444_P },
    { BC_YUV411P,      GAVL_YUV_411_P },
    { BC_YUV422P16,    GAVL_YUV_422_P_16 },
    { BC_YUV444P16,    GAVL_YUV_444_P_16 },
  };

static gavl_pixelformat_t
pixelformat_lqt_2_gavl(int pixelformat)
  {
  int i;
  for(i = 0; i < sizeof(pixelformats) / sizeof(pixelformats[0]); i++)
    {
    if(pixelformats[i].lqt == pixelformat)
      return pixelformats[i].gavl;
    }
  return GAVL_PIXELFORMAT_NONE;
  }

static const struct
  {
  gavl_channel_id_t gavl;
  lqt_channel_t lqt;
  }
channels[] =
  {
    { GAVL_CHID_NONE,               LQT_CHANNEL_UNKNOWN            },
    { GAVL_CHID_FRONT_CENTER,       LQT_CHANNEL_FRONT_CENTER       },
    { GAVL_CHID_FRONT_LEFT,         LQT_CHANNEL_FRONT_LEFT         },
    { GAVL_CHID_FRONT_RIGHT,        LQT_CHANNEL_FRONT_RIGHT        },
    { GAVL_CHID_FRONT_CENTER_LEFT,  LQT_CHANNEL_FRONT_CENTER_LEFT  },
    { GAVL_CHID_FRONT_CENTER_RIGHT, LQT_CHANNEL_FRONT_CENTER_RIGHT },
    { GAVL_CHID_REAR_LEFT,          LQT_CHANNEL_BACK_LEFT          },
    { GAVL_CHID_REAR_RIGHT,         LQT_CHANNEL_BACK_RIGHT         },
    { GAVL_CHID_REAR_CENTER,        LQT_CHANNEL_BACK_CENTER        },
    { GAVL_CHID_SIDE_LEFT,          LQT_CHANNEL_SIDE_LEFT          },
    { GAVL_CHID_SIDE_RIGHT,         LQT_CHANNEL_SIDE_RIGHT         },
    { GAVL_CHID_LFE,                LQT_CHANNEL_LFE                },
    { GAVL_CHID_AUX,                LQT_CHANNEL_UNKNOWN            },
  };

static gavl_channel_id_t
channel_lqt_2_gavl(lqt_channel_t ch)
  {
  int i;
  for(i = 0; i < sizeof(channels) / sizeof(channels[0]); i++)
    {
    if(channels[i].lqt == ch)
      return channels[i].gavl;
    }
  return GAVL_CHID_NONE;
  }

static lqt_channel_t
channel_gavl_2_lqt(gavl_channel_id_t ch)
  {
  int i;
  for(i = 0; i < sizeof(channels) / sizeof(channels[0]); i++)
    {
    if(channels[i].gavl == ch)
      return channels[i].lqt;
    }
  return LQT_CHANNEL_UNKNOWN;
  }

void lqt_gavl_add_audio_track(quicktime_t * file,
                               gavl_audio_format_t * format,
                               lqt_codec_info_t * codec)
  {
  int i;
  const lqt_channel_t * chans_1;
  lqt_channel_t * chans_2;
  
  int track = quicktime_audio_tracks(file);
  
  lqt_add_audio_track(file, format->num_channels, format->samplerate,
                      16, codec);

  format->sample_format =
    sampleformat_lqt_2_gavl(lqt_get_sample_format(file, track));
  format->interleave_mode = GAVL_INTERLEAVE_ALL;

  /* Negotiate the channel setup */

  /* 1st try: The codec already knows (we cannot change that) */
  chans_1 = lqt_get_channel_setup(file, track);
  if(chans_1)
    {
    for(i = 0; i < format->num_channels; i++)
      format->channel_locations[i] = channel_lqt_2_gavl(chans_1[i]);
    }
  else
    {
    /* Set our channel setup */
    chans_2 = calloc(format->num_channels, sizeof(*chans_2));
    for(i = 0; i < format->num_channels; i++)
      chans_2[i] = channel_gavl_2_lqt(format->channel_locations[i]);
    lqt_set_channel_setup(file, track, chans_2);
    free(chans_2);

    /* Copy reordered setup back */
    chans_1 = lqt_get_channel_setup(file, track);
    for(i = 0; i < format->num_channels; i++)
      format->channel_locations[i] = channel_lqt_2_gavl(chans_1[i]);
    }
  }

void lqt_gavl_add_video_track(quicktime_t * file,
                               gavl_video_format_t * format,
                               lqt_codec_info_t * codec)
  {
  int track = quicktime_video_tracks(file);

  lqt_add_video_track(file, format->image_width, format->image_height,
                      format->frame_duration, format->timescale,
                      codec);
  lqt_set_pixel_aspect(file, track, format->pixel_width, format->pixel_height);
  lqt_set_interlace_mode(file, track,
                         interlace_mode_gavl_2_lqt(format->interlace_mode));
  
  format->pixelformat = pixelformat_lqt_2_gavl(lqt_get_cmodel(file, track));
  }



int lqt_gavl_encode_video(quicktime_t * file, int track,
                           gavl_video_frame_t * frame, uint8_t ** rows)
  {
  int i, height;
  int result;
  
  if(lqt_colormodel_is_planar(lqt_get_cmodel(file, track)))
    {
    lqt_set_row_span(file, track, frame->strides[0]);
    lqt_set_row_span_uv(file, track, frame->strides[1]);
    result = lqt_encode_video(file, frame->planes, track, frame->timestamp);
    }
  else
    {
    height = quicktime_video_height(file, track);
    for(i = 0; i < height; i++)
      {
      lqt_set_row_span(file, track, frame->strides[0]);
      rows[i] = frame->planes[0] + i * frame->strides[0];
      }
    result = lqt_encode_video(file, rows, track, frame->timestamp);
    }
  return result;
  }

void lqt_gavl_encode_audio(quicktime_t * file, int track,
                           gavl_audio_frame_t * frame)
  {
  lqt_encode_audio_raw(file, frame->samples.s_8, frame->valid_samples,
                       track);
  }

/* Decoding */

int lqt_gavl_get_audio_format(quicktime_t * file,
                              int track,
                              gavl_audio_format_t * format)
  {
  int i;
  const lqt_channel_t * channel_setup;

  if(track >= quicktime_audio_tracks(file) ||
     track < 0)
    return 0;
  
  format->num_channels = quicktime_track_channels(file, track);
  format->samplerate = quicktime_sample_rate(file, track);
  format->sample_format =
    sampleformat_lqt_2_gavl(lqt_get_sample_format(file, track));
  format->interleave_mode = GAVL_INTERLEAVE_ALL;

  format->samples_per_frame = 1024; // Meaningless but better than 0
  channel_setup = lqt_get_channel_setup(file, track);

  if(channel_setup)
    {
    for(i = 0; i < format->num_channels; i++)
      {
      format->channel_locations[i] = channel_lqt_2_gavl(channel_setup[i]);
      }
    }
  else
    gavl_set_channel_setup(format);
  return 1;
  }

int lqt_gavl_get_video_format(quicktime_t * file,
                              int track,
                              gavl_video_format_t * format, int encode)
  {
  int constant_framerate;
  if(track >= quicktime_video_tracks(file) ||
     track < 0)
    return 0;

  format->image_width = quicktime_video_width(file, track);
  format->image_height = quicktime_video_height(file, track);

  format->frame_width = format->image_width;
  format->frame_height = format->image_height;

  lqt_get_pixel_aspect(file, track, &format->pixel_width,
                       &format->pixel_height);

  format->timescale = lqt_video_time_scale(file, track);
  format->frame_duration = lqt_frame_duration(file, track,
                                              &constant_framerate);

  if(encode)
    {
    if((lqt_get_file_type(file) & (LQT_FILE_AVI|LQT_FILE_AVI_ODML)))
      format->framerate_mode = GAVL_FRAMERATE_CONSTANT;
    }
  else
    {
    if(!constant_framerate)
      format->framerate_mode = GAVL_FRAMERATE_VARIABLE;
    else
      format->framerate_mode = GAVL_FRAMERATE_CONSTANT;
    }
  
  format->chroma_placement =
    chroma_placement_lqt_2_gavl(lqt_get_chroma_placement(file, track));
  
  format->interlace_mode =
    interlace_mode_lqt_2_gavl(lqt_get_interlace_mode(file, track));

  format->pixelformat = pixelformat_lqt_2_gavl(lqt_get_cmodel(file, track));
  
  return 1;
  }

int lqt_gavl_decode_video(quicktime_t * file, int track,
                          gavl_video_frame_t * frame, uint8_t ** rows)
  {
  int i, height;
  if(quicktime_video_position(file, track) >=
     quicktime_video_length(file, track))
    return 0;
  
  frame->timestamp = lqt_frame_time(file, track);



  if(lqt_colormodel_is_planar(lqt_get_cmodel(file, track)))
    {
    lqt_set_row_span(file, track, frame->strides[0]);
    lqt_set_row_span_uv(file, track, frame->strides[1]);
    lqt_decode_video(file, frame->planes, track);
    }
  else
    {
    height = quicktime_video_height(file, track);
    for(i = 0; i < height; i++)
      {
      lqt_set_row_span(file, track, frame->strides[0]);
      rows[i] = frame->planes[0] + i * frame->strides[0];
      }
    lqt_decode_video(file, rows, track);
    }
  return 1;
  }

int lqt_gavl_decode_audio(quicktime_t * file, int track,
                          gavl_audio_frame_t * frame,
                          int samples)
  {
  frame->timestamp = quicktime_audio_position(file, track);
  lqt_decode_audio_raw(file, frame->samples.s_8, samples, track);
  frame->valid_samples = lqt_last_audio_position(file, track) - frame->timestamp;
  return frame->valid_samples;
  }

void lqt_gavl_seek(quicktime_t * file, gavl_time_t * time)
  {
  int imax;
  int i;
  int64_t video_time_save  = -1;
  int video_timescale_save = -1;

  int64_t time_scaled;
  int timescale;

  /* We synchronize to the first video track */

  imax = quicktime_video_tracks(file);

  for(i = 0; i < imax; i++)
    {
    timescale = lqt_video_time_scale(file, i);
    time_scaled = gavl_time_scale(timescale, *time);
    lqt_seek_video(file, i, time_scaled);
    if(!i)
      {
      video_timescale_save = timescale;
      video_time_save      = time_scaled;
      }
    }

  if(video_time_save >= 0)
    *time = gavl_time_unscale(video_timescale_save, video_time_save);

  imax = quicktime_audio_tracks(file);
  for(i = 0; i < imax; i++)
    {
    timescale = quicktime_sample_rate(file, i);
    time_scaled = gavl_time_scale(timescale, *time);
    quicktime_set_audio_position(file, time_scaled, i);
    }

  imax = lqt_text_tracks(file);
  for(i = 0; i < imax; i++)
    {
    if(lqt_is_chapter_track(file, i))
      continue;
    
    timescale = lqt_text_time_scale(file, i);
    time_scaled = gavl_time_scale(timescale, *time);
    lqt_set_text_time(file, i, time_scaled);
    }
  }

uint8_t ** lqt_gavl_rows_create(quicktime_t * file, int track)
  {
  uint8_t ** ret;

  if(lqt_colormodel_is_planar(lqt_get_cmodel(file, track)))
    return (uint8_t**)0;
  
  ret = malloc(sizeof(*ret) * quicktime_video_height(file, track));
  return ret;
  }


void lqt_gavl_rows_destroy(uint8_t** rows)
  {
  if(rows)
    free(rows);
  }

gavl_time_t lqt_gavl_duration(quicktime_t * file)
  {
  gavl_time_t ret = 0, time;
  int i, imax;
  int timescale;
  int64_t time_scaled;
  
  imax = quicktime_audio_tracks(file);
  for(i = 0; i < imax; i++)
    {
    timescale = quicktime_sample_rate(file, i);
    time_scaled = quicktime_audio_length(file, i);

    time = gavl_time_unscale(timescale, time_scaled);
    if(ret < time)
      ret = time;
    }

  imax = quicktime_video_tracks(file);
  for(i = 0; i < imax; i++)
    {
    timescale = lqt_video_time_scale(file, i);
    time_scaled = lqt_video_duration(file, i);
    
    time = gavl_time_unscale(timescale, time_scaled);
    if(ret < time)
      ret = time;
    }
  return ret;
  }
