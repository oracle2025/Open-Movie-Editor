#ifndef _LQT_BITRATE_PARAMS_H_
#define _LQT_BITRATE_PARAMS_H_

char* lqt_bitrate_params[][2] = {
	{ "ffmpeg_mpg4", "ff_bit_rate_video" },
	{ "ffmpeg_msmpeg4v3", "ff_bit_rate_video" },
	{ "ffmpeg_h263", "ff_bit_rate_video" },
	{ "ffmpeg_h263p", "ff_bit_rate_video" },
	{ "ffmpeg_mjpg", "ff_bit_rate_video" },
	{ "x264", "x264_i_bitrate" },
	{ "ffmpeg_mp2", "bit_rate_audio" },
	{ "ffmpeg_mp2", "bit_rate_audio" },
	{ "lame", "mp3_bitrate" },
	{ "vorbis", "vorbis_bitrate" },
	{ "vorbis_qt", "vorbis_bitrate" },
	{ "faac", "faac_bitrate" },
	0L
};

#endif /* _LQT_BITRATE_PARAMS_H_ */

