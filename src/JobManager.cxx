/*  JobManager.cxx
 *
 *  Copyright (C) 2008, 2009 Richard Spindler <richard.spindler AT gmail.com>
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

#include <cassert>
#include <iostream>

#include <FL/Fl.H>

#include "JobManager.H"

namespace nle
{

IJobManager* g_jobManager = 0;

JobFifo::JobFifo()
{
	m_first = 0;
	m_last = 0;
}
JobFifo::~JobFifo()
{
	Job* i;
	while ( i = pop() ) {
		delete i;
	}
}
void JobFifo::append( Job* job )
{
	if ( !m_first ) {
		assert( !m_last );
		m_first =  m_last = job;
	} else {
		m_last->next = job;
		m_last = job;
	}
}
Job* JobFifo::pop()
{
	if ( !m_first ) {
		return 0;
	}
	Job* result = m_first;
	m_first = m_first->next;
	result->next = 0; // don't pass around dangling references
	if  ( !m_first ) {
		//Fifo is now empty, so set last Item to 0
		m_last = 0;
	}
	return result;
}

static void process_callback_JobManagerIdle( void* data )
{
	JobManagerIdle* jm = (JobManagerIdle*)data;
	jm->process();
}

JobManagerIdle::JobManagerIdle()
{
	g_jobManager = this;
	m_runningJob = 0;
	m_running = false;
}
JobManagerIdle::~JobManagerIdle()
{
	stop();
	if ( m_runningJob ) {
		delete m_runningJob;
		m_runningJob = 0;
	}
}
void JobManagerIdle::process()
{
	double percentage;
	if ( m_runningJob && m_runningJob->process( percentage ) ) {
		return;
	}
	if ( m_runningJob && m_runningJob->done() ) {
		m_runningJob->callJobDone(); // The JobDone Listener is supposed to delete the Job
		m_runningJob = 0;
	}
	if ( !m_runningJob ) {
		m_runningJob = m_jobs.pop();
	}
	if ( !m_runningJob ) {
		stop();
	}
}
void JobManagerIdle::submitJob( Job* job_description )
{
	m_jobs.append( job_description );
	start();
}
void JobManagerIdle::start()
{
	if ( m_running ) {
		return;
	}
	m_running = true;
	Fl::add_idle( process_callback_JobManagerIdle, this );
}
void JobManagerIdle::stop()
{
	if ( !m_running ) {
		return;
	}
	m_running = false;
	Fl::remove_idle( process_callback_JobManagerIdle, this );
}

} /* namespace nle */
