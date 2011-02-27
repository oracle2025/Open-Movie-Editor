
#include "AudioThreadedRingbuffer.H"
#include "IAudioReader.H"
#include <glib.h>
#include <string.h>
#include <cerrno>


#define FRAMES 4096
#define BYTES_PER_FRAME ( FRAMES * sizeof(float) * 2 )
#define DEFAULT_RB_SIZE 16384

namespace nle
{

static void* start_audio_reader_thread( void* data )
{
	AudioThreadedRingbuffer* reader = (AudioThreadedRingbuffer*)data;
	reader->run();
	return 0;
}


AudioThreadedRingbuffer::AudioThreadedRingbuffer( IAudioReader* reader )
	: m_reader( reader )
{
	m_ringBuffer = jack_ringbuffer_create( DEFAULT_RB_SIZE * sizeof(float) * 2 );
	m_buffer = new float[ FRAMES * 2 ];
	m_running = true;
	m_ready = false;
	m_seekPending = true;
	m_sample = 0;
	m_seekTarget = -1;
	pthread_mutex_init( &m_readMutex, 0 );
	pthread_cond_init( &m_readCondition, 0 );
	pthread_create( &m_thread, NULL, start_audio_reader_thread, (void*)this );

}
AudioThreadedRingbuffer::~AudioThreadedRingbuffer()
{
	pthread_mutex_lock( &m_readMutex );
	m_running = false;
	pthread_cond_signal( &m_readCondition );
	pthread_mutex_unlock( &m_readMutex );
	pthread_join( m_thread, 0 );
	pthread_mutex_destroy( &m_readMutex );
	pthread_cond_destroy( &m_readCondition );
	jack_ringbuffer_free( m_ringBuffer );
	delete [] m_buffer;
}
bool AudioThreadedRingbuffer::ready()
{
	return g_atomic_int_get( &m_ready );
}

int AudioThreadedRingbuffer::fillBuffer( float* buffer, unsigned long frames ) /* Called from Realtime Thread */
{
	if ( !g_atomic_int_get( &m_ready ) ) {
		memset( buffer, 0, sizeof(float) * frames * 2 );
		return 0;
	}
	m_seekTarget = -1;
	int bytes_read;
	unsigned int frames_read;
	bytes_read = jack_ringbuffer_read( m_ringBuffer, (char*)buffer, frames * sizeof(float) * 2 );
	frames_read = bytes_read / ( 2 * sizeof(float) );
	if ( frames_read < frames ) {
		memset( buffer + ( frames_read * 2 ), 0, sizeof(float) * ( frames - frames_read ) * 2 );
	}
	ping();
	return frames_read;
}

void AudioThreadedRingbuffer::seek( int64_t sample ) /* Called from Realtime Thread */
{
	if ( !g_atomic_int_get( &m_ready ) ) {
		// ERROR, seek without waiting for pending seek
		return;
	}
	if ( sample == m_seekTarget ) {
		return;
	}
	m_sample = sample;
	m_seekTarget = sample;
	g_atomic_int_set( &m_ready, false );
	g_atomic_int_set( &m_seekPending, true );
	ping();
}

void AudioThreadedRingbuffer::ping() /* Called from Realtime Thread */
{
	if ( pthread_mutex_trylock( &m_readMutex ) != EBUSY ) {
		pthread_cond_signal( &m_readCondition );
		pthread_mutex_unlock( &m_readMutex );
	}
}
void AudioThreadedRingbuffer::perform_seek()
{
	m_reader->sampleseek( 1, m_sample );
	jack_ringbuffer_reset( m_ringBuffer );
}

void AudioThreadedRingbuffer::run() /* Called from Disk Thread once */
{
	int frames_read;
	pthread_mutex_lock( &m_readMutex );
	while( m_running ) {
		if ( g_atomic_int_get( &m_seekPending ) ) {
			perform_seek();
			while( jack_ringbuffer_write_space( m_ringBuffer ) >= BYTES_PER_FRAME ) {
				frames_read = m_reader->fillBuffer( m_buffer, FRAMES );
				m_reader->sampleseek( 0, frames_read );
				if ( frames_read == 0 ) { /* ? */ }
				jack_ringbuffer_write( m_ringBuffer, (char*)m_buffer, BYTES_PER_FRAME );
			}
			g_atomic_int_set( &m_seekPending, false );
			g_atomic_int_set( &m_ready, true );
		}
		while( jack_ringbuffer_write_space( m_ringBuffer ) >= BYTES_PER_FRAME ) {
			frames_read = m_reader->fillBuffer( m_buffer, FRAMES );
			m_reader->sampleseek( 0, frames_read );
			if ( frames_read == 0 ) { /* ? */ }
			jack_ringbuffer_write( m_ringBuffer, (char*)m_buffer, BYTES_PER_FRAME );
		}
		pthread_cond_wait( &m_readCondition, &m_readMutex );
	}
	pthread_mutex_unlock( &m_readMutex );
}

} /* namespace nle */
