/*  EncodingPreset.H
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _ENCODING_PRESET_H_
#define _ENCODING_PRESET_H_
namespace nle{
	class EncodingPreset;
}


#include "RenderDialog.H"
#include "fps_definitions.H"
#include <lqt.h>

class CodecOptions;
class Fl_Choice;
class TiXmlElement;

namespace nle
{

void setAudioCodecMenu( Fl_Choice* menu, int type );
void setVideoCodecMenu( Fl_Choice* menu, int type );

void setCodecInfo( CodecOptions* dialog, void* data );

void setCodecParameter( CodecOptions* dialog, void* data );

typedef struct _param_list_node {
	struct _param_list_node *next;
	lqt_parameter_value_t value;
	lqt_parameter_info_t * info;
} param_node;

typedef struct _codec_list_node {
	struct _codec_list_node *next;
	lqt_codec_info_t* codecInfo;
	param_node* parameters;
} codec_node;

typedef enum {
	PV_INT,
	PV_STRING
} pv_type;

class ParameterValue
{
	public:
		ParameterValue( const char* v );
		ParameterValue( int v );
		~ParameterValue();
		void get( lqt_parameter_value_t& val );
	private:
		pv_type m_type;
		char* m_value_string;
		int m_value_int;
};

} /* namespace nle */




#include "video_format.H"
#include <string>
#include <lqt.h>
#include <lqt_version.h>


namespace nle
{
class IVideoFileWriter;

class EncodingPreset
{
	public:
		EncodingPreset();
		EncodingPreset( EncodingPreset* preset );
		~EncodingPreset();
		IVideoFileWriter* getFileWriter( const char* filename );
		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );
		void setFormat( video_format* format );
		void getFormat( video_format* format );
		void clearCodecValues();
		bool m_readonly;
		bool m_avi_odml;
#if (LQT_CODEC_API_VERSION & 0xffff) > 6
		lqt_file_type_t m_file_type;
#endif
		/* ----copied from CodecParameters----*/
		void writeXML_CodecParameters( TiXmlElement* );
		void readXML_CodecParameters( TiXmlElement* );
		void setVideoCodec( lqt_codec_info_t* info );
		void setAudioCodec( lqt_codec_info_t* info );
		void setVideoParameter( const char* key, ParameterValue& value );
		void setAudioParameter( const char* key, ParameterValue& value );
		lqt_parameter_value_t getVideoParameter( const char* key );
		lqt_parameter_value_t getAudioParameter( const char* key );
		void set2( quicktime_t* qt );
		void construct_CodecParameters( lqt_codec_info_t** audio, lqt_codec_info_t** video );
		void construct_CodecParameters( lqt_codec_info_t** audio, lqt_codec_info_t** video, EncodingPreset* copy );
		/* ----copied from CodecParameters----*/
		int bitrate();
		int audiobitrate();
		void bitrate( int rate );
		void audiobitrate( int rate );
	private:
		video_format m_format;
		lqt_codec_info_t** lqt_audio_codecs;
		lqt_codec_info_t** lqt_video_codecs;
		
		/* ----copied from CodecParameters----*/
		void init( lqt_codec_info_t** audio, lqt_codec_info_t** video );
		codec_node* m_audioCodecs;
		codec_node* m_videoCodecs;
		codec_node* m_currentAudioCodec;
		codec_node* m_currentVideoCodec;
		/* ----copied from CodecParameters----*/
};
	
} /* namespace nle */


#endif /* _ENCODING_PRESET_H_ */

