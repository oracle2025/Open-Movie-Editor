#ifndef _AUDIO_THREADED_RINGBUFFER_H_
#define _AUDIO_THREADED_RINGBUFFER_H_

#include <pthread.h>
#include <jack/ringbuffer.h>

namespace nle
{

class IAudioReader;

class AudioThreadedRingbuffer
{
	public:
		AudioThreadedRingbuffer( IAudioReader* reader );
		~AudioThreadedRingbuffer();
		void seek( int64_t sample ); /* MUST not be called when not "ready" */
		bool ready();
		void ping();
		int fillBuffer( float* buffer, unsigned long frames );
		void run();
	private:
		void perform_seek();
		bool m_running;
		pthread_mutex_t m_readMutex;
		pthread_cond_t m_readCondition;
		pthread_t m_thread;
		jack_ringbuffer_t* m_ringBuffer;
		float* m_buffer;
		int m_ready;
		int m_seekPending;
		int64_t m_seekTarget;
		int64_t m_sample;
		IAudioReader* m_reader;
};

} /* namespace nle */

#endif /* _AUDIO_THREADED_RINGBUFFER_H_ */
