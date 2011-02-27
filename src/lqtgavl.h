/*****************************************************************

  lqtgavl.h

  Copyright (c) 2005 by Burkhard Plaum - plaum@ipf.uni-stuttgart.de

  http://gmerlin.sourceforge.net

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.

*****************************************************************/

#ifndef _LQTGAVL_H_
#define _LQTGAVL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gavl/gavl.h>
#include <lqt.h>

/*! \mainpage

 \section Introduction
 This is the API documentation for lqtgavl, a gavl wrapper for libquicktime.
 Click Modules (on top of the page) to get to the main API index.
 Here, you find just some general blabla :)

 \section Usage
 The recommended usage is to copy the files include/lqtgavl.h
 and lib/lqtgavl.c into your sourcetree. This is to small to distribute as
 an external library.
*/

/** \defgroup encode Encoding related functions
 */

/** \defgroup decode Decoding related functions
 */

/** \defgroup rows Frame rows pointers
 *
 *  Video frames in libquicktime and gavl are compatible for planar formats.
 *  For packed formats however, we need an additional array for storing the
 *  rows. We could allocate and free them dynamically, causing a lot of overhead.
 *  Therefore, we supply 2 functions for creating and freeing the row pointers so
 *  they can be reused.
 */

/** \ingroup rows
 *  \brief Creeate a row pointer array.
 *  \param file A quicktime handle
 *  \param track Track index (starting with 0)
 *  \returns A row pointer array, you can pass to \ref lqt_gavl_encode_video or
 *  \ref lqt_gavl_decode_video.
 */

uint8_t ** lqt_gavl_rows_create(quicktime_t * file, int track);

/** \ingroup rows
    \brief Free rows array
    \param rows Row array crated with \ref lqt_gavl_rows_create.
*/

void lqt_gavl_rows_destroy(uint8_t** rows);


/** \ingroup encode
 *  \brief Set up an audio stream for encoding
 *  \param file A quicktime handle
 *  \param format Audio format
 *  \param codec The codec to use
 *
 * This function sets up an audio stream for encoding.
 * This function will change the format parameter according to
 * what you must pass to the encode calls. If the format is different
 * from the source format, you need a \ref gavl_audio_converter_t.
 */


void lqt_gavl_add_audio_track(quicktime_t * file,
                               gavl_audio_format_t * format,
                               lqt_codec_info_t * codec);

/** \ingroup encode
 *  \brief Set up a video stream for encoding
 *  \param file A quicktime handle
 *  \param format Video format
 *  \param codec The codec to use
 *
 * This function sets up a video stream for encoding.
 * This function will change the format parameter according to
 * what you must pass to the encode calls. If the format is different
 * from the source format, you need a \ref gavl_video_converter_t.
 */

void lqt_gavl_add_video_track(quicktime_t * file,
                               gavl_video_format_t * format,
                               lqt_codec_info_t * codec);

/* Encode audio/video */

/** \ingroup encode
 *  \brief Encode a video frame
 *  \param file A quicktime handle
 *  \param track Track index (starting with 0)
 *  \param frame Video frame
 *  \param rows Rows (created with \ref lqt_gavl_rows_create)
 *  \returns 1 if a frame could be encoded, 0 else
 *
 *  Pass one audio frame to libquicktime for encoding.
 *  The format must be the same as returned by \ref lqt_gavl_add_audio_track.
 *  The samples_per_frame member of the frame can be larger than
 *  the samples_per_frame member of the format.
 */

int lqt_gavl_encode_video(quicktime_t * file, int track,
                           gavl_video_frame_t * frame, uint8_t ** rows);

/** \ingroup encode
 *  \brief Encode a video frame
 *  \param file A quicktime handle
 *  \param track Track index (starting with 0)
 *  \param frame Video frame
 *
 *  Pass one video frame to libquicktime for encoding.
 *  The format must be the same as returned by \ref lqt_gavl_add_video_track.
 *  If the time_scaled member of the frame is no multiple of the frame_duration
 *  the framerate is variable.
 */


void lqt_gavl_encode_audio(quicktime_t * file, int track,
                           gavl_audio_frame_t * frame);

/* Get formats for decoding */

/** \ingroup decode
 *  \brief Get the audio format for decoding
 *  \param file A quicktime handle
 *  \param track Track index (starting with 0)
 *  \param format Returns the format.
 *  \returns 1 on success, 0 if there is no such track.
 */

int lqt_gavl_get_audio_format(quicktime_t * file,
                              int track,
                              gavl_audio_format_t * format);

/** \ingroup decode
 *  \brief Get the video format for decoding
 *  \param file A quicktime handle
 *  \param track Track index (starting with 0)
 *  \param format Returns the format.
 *  \param encode Set to 1 if encoding, 0 for decoding
 *  \returns 1 on success, 0 if there is no such track.
 */

int lqt_gavl_get_video_format(quicktime_t * file,
                              int track,
                              gavl_video_format_t * format, int encode);

/* Decode audio/video */

/** \ingroup decode
 *  \brief Decode one video frame
 *  \param file A quicktime handle
 *  \param track Track index (starting with 0)
 *  \param frame The frame
 *  \param rows Rows (created with \ref lqt_gavl_rows_create)
 *  \returns 1 on success, 0 for EOF.
 **/

int lqt_gavl_decode_video(quicktime_t * file,
                          int track,
                          gavl_video_frame_t * frame, uint8_t ** rows);

/** \ingroup decode
 *  \brief Decode one audio frame
 *  \param file A quicktime handle
 *  \param track Track index (starting with 0)
 *  \param frame The frame
 *  \param samples Number of samples to decode.
 *  \returns The real number of decoded samples, 0 for EOF.
 **/

int lqt_gavl_decode_audio(quicktime_t * file,
                          int track,
                          gavl_audio_frame_t * frame, int samples);

/** \ingroup decode
 *  \brief Seek all tracks to a specific point
 *  \param file A quicktime handle
 *  \param time The time to seek to.
 *
 * This call sets the time argument to the real time, we have now.
 * It might be changed if you seek between 2 video frames.
 **/

void lqt_gavl_seek(quicktime_t * file, gavl_time_t * time);
  
                   
/** \ingroup decode
 *  \brief Get the total duration
 *  \param file A quicktime handle
 *  \returns The duration
 *
 * Return the whole duration of the file as a \ref gavl_time_t
 **/

gavl_time_t lqt_gavl_duration(quicktime_t * file);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _LQTGAVL_H_ */
