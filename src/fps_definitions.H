#ifndef _FPS_DEFINITIONS_H_
#define _FPS_DEFINITIONS_H_


typedef struct
{
	int w;
	int h;
	int aspect_w;
	int aspect_h;
	float aspect;
	int analog_blank;
} render_frame_size;

typedef struct
{
	int frame_duration;
	int timescale;
	int audio_frames_per_chunk;
	int video_frames_per_chunk;
} render_fps_chunks;

typedef enum {
	INTERLACE_PROGRESSIVE = 0,
	INTERLACE_TOP_FIELD_FIRST = 1,
	INTERLACE_BOTTOM_FIELD_FIRST = 2,
	INTERLACE_DEVIDED_FIELDS = 3 /* Top Frame is first */
} interlace_state;


extern render_fps_chunks fps25x48000;
extern render_fps_chunks fps29_97x48000;
extern render_fps_chunks fps24x48000;
extern render_fps_chunks fps15x48000;
extern render_fps_chunks fps50x48000;
extern render_fps_chunks fps60x48000;

extern render_frame_size fs720x576;
extern render_frame_size fs768x576;
extern render_frame_size fs384x288;
extern render_frame_size fs640x480;
extern render_frame_size fs320x240;


#endif /* _FPS_DEFINITIONS_H_ */

