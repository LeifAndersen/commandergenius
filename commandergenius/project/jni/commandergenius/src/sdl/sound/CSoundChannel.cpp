/*
 * CSoundChannel.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#include "CSoundChannel.h"

CSoundChannel::CSoundChannel() {
	m_current_sound = 0;
	m_freq_corr = 0;	// used for correcting PC-Speaker sampling for different frequencies
	m_sound_ptr = 0;
	m_sound_timer = 0;
	m_sound_playing = false;
	m_waveState = 0;
	m_hq = false;
	m_sound_paused = true;
	m_sound_forced = false;
	m_pSoundSlot = NULL;
	
	m_desiredfreq = 0;     	// current desired frequency in hz
	m_changerate = 0;      	// frequency in samples (calculated)
	m_freqtimer = 0;       	// time when to change waveform state
	m_waveState = 0;       	// current position of the output waveform
	m_balance = 0;
	
	m_format	= 0;
	m_volume	= 0;
	m_silence	= 0;
	m_waveout	= 0;
	m_wavein	= 0;
}

CSoundChannel::~CSoundChannel() {
}

void CSoundChannel::setFrequencyCorrection(int freq)
{
	switch (freq)
	{
		case 44100: m_freq_corr = 15; break;
		case 22050: m_freq_corr = 14; break;
		case 11025: m_freq_corr = 12; break;
		default: m_freq_corr = 0; break;
	}
}

void CSoundChannel::stopSound(void)
{
	m_current_sound = 0;
    m_sound_ptr = 0;
    m_sound_timer = 0;
    m_sound_playing = false;
    m_waveState = m_wavein;
    m_freqtimer = 0;
}

void CSoundChannel::setFormat( Uint16 format )
{
	m_format = format;
	switch( m_format )
	{
		case AUDIO_U8:
			m_volume	= WAVEFORM_VOLUME_8;
			m_silence	= WAVE_SILENCE_U8;
			m_waveout	= WAVE_OUT_U8;
			m_wavein	= WAVE_IN_U8;
			break;
		case AUDIO_S8:
			m_volume	= WAVEFORM_VOLUME_8;
			m_silence	= WAVE_SILENCE_S8;
			m_waveout	= WAVE_OUT_S8;
			m_wavein	= WAVE_IN_S8;
			break;
		case AUDIO_U16:
			m_volume	= WAVEFORM_VOLUME_16;
			m_silence	= WAVE_SILENCE_U16;
			m_waveout	= WAVE_OUT_U16;
			m_wavein	= WAVE_IN_U16;
			break;
		case AUDIO_S16:
			m_volume	= WAVEFORM_VOLUME_16;
			m_silence	= WAVE_SILENCE_S16;
			m_waveout	= WAVE_OUT_S16;
			m_wavein	= WAVE_IN_S16;
			break;
	}
	
	m_waveState = m_wavein;
}


void CSoundChannel::setupSound(unsigned short current_sound,
							   unsigned int sound_timer,
							   bool playing,
							   unsigned int freqtimer,
							   bool sound_forced,
							   Uint16 format)
{
	setFormat( format );
	m_current_sound = current_sound;
	m_sound_timer = sound_timer;
	m_sound_playing = playing;
	m_waveState = m_wavein;
	m_freqtimer = freqtimer;
	m_sound_forced = sound_forced;
}

// generates len bytes of waveform for the channel.
void CSoundChannel::generateWaveform16(Uint8 *waveform, unsigned int len, int frequency, bool stereo )
{ 
	int i;
	int waittime; // How much do we divide wait time for change?
	unsigned int halffreq;
	unsigned int index;
	char firsttime;
	Uint16 *WaveBuffer;
	
	WaveBuffer = new Uint16[len];
	
	halffreq = (frequency>>1);
	
	if(m_freq_corr == 16)
		waittime = (frequency / SLOW_RATE);
	else
		waittime = (((m_freq_corr*frequency)>>4) / SLOW_RATE);
	
	// setup so we process a new byte of the sound first time through
	firsttime = 1;
	
	for(index=0 ; index<len ; index++)
	{
		if (!m_sound_timer || firsttime)
		{
			// get new frequency and compute how fast we have to
			// change the wave data
			if(m_sound_ptr < m_pSoundSlot[m_current_sound].getSoundlength())
				m_desiredfreq = m_pSoundSlot[m_current_sound].getSoundData()[m_sound_ptr];
			else
				m_desiredfreq = 0xffff;
			
			if (m_desiredfreq==0xffff)
			{  // end of sound...fill rest of buffer with silence
				for(;index<len;index++)
				{
					WaveBuffer[index] = m_silence;
				}
				m_sound_playing = false;
				
				memcpy(waveform, WaveBuffer,len*sizeof(Uint16));
				
				m_sound_ptr = 0;
				
				// Only copy the rest of the remaing buffer space and return.
				delete[] WaveBuffer;
				
				return;
			}
			else if (m_desiredfreq == 0x0000)
			{
				m_waveState = m_waveout;
			}
			else
			{  // compute change rate
				m_changerate = (halffreq / m_desiredfreq);
			}
			
			m_sound_ptr++;
			if (!firsttime) m_sound_timer = waittime;
			firsttime = 0;
		}
		
		if (m_sound_timer) m_sound_timer--;
		
		if (m_desiredfreq==0x0000)
		{   // silence
			WaveBuffer[index] = m_silence;
		}
		else
		{
			// time to change waveform state?
			if (m_freqtimer > m_changerate)
			{  // toggle waveform, generating a square wave
				i=0;
				
				if(m_sound_ptr > 0)
					i=m_volume;
				
				if (m_waveState == m_silence-i)
					m_waveState = m_silence+i;
				else
					m_waveState = m_silence-i;
				
				m_freqtimer = 0;
			}
			else m_freqtimer++;
			
			// put wave data into buffer
			WaveBuffer[index] = m_waveState;
		}
		
		if(stereo) // Primitive stereo transformation
		{
			if(index < len)
			{
				index++;
				WaveBuffer[index] = m_waveState;
			}
		}
	}
	memcpy(waveform, WaveBuffer,len*sizeof(Uint16));
	delete[] WaveBuffer;
}

// generates len bytes of waveform for the channel.
void CSoundChannel::generateWaveform8(Uint8 *waveform, unsigned int len, int frequency, bool stereo )
{ 
	int i;
	int waittime; // How much do we divide wait time for change?
	unsigned int halffreq;
	unsigned int index;
	char firsttime;
	Uint8 *WaveBuffer;
	
	WaveBuffer = new Uint8[len];
	
	halffreq = (frequency>>1);
	
	if(m_freq_corr == 16)
		waittime = (frequency / SLOW_RATE);
	else
		waittime = (((m_freq_corr*frequency)>>4) / SLOW_RATE);
	
	// setup so we process a new byte of the sound first time through
	firsttime = 1;
	
	for(index=0 ; index<len ; index++)
	{
		if (!m_sound_timer || firsttime)
		{
			// get new frequency and compute how fast we have to
			// change the wave data
			if(m_sound_ptr < m_pSoundSlot[m_current_sound].getSoundlength())
				m_desiredfreq = m_pSoundSlot[m_current_sound].getSoundData()[m_sound_ptr];
			else
				m_desiredfreq = 0xffff;
			
			if (m_desiredfreq==0xffff)
			{  // end of sound...fill rest of buffer with silence
				for(;index<len;index++)
				{
					WaveBuffer[index] = m_silence;
				}
				m_sound_playing = false;
				
				memcpy(waveform, WaveBuffer,len*sizeof(Uint8));
				
				m_sound_ptr = 0;
				
				// Only copy the rest of the remaing buffer space and return.
				delete[] WaveBuffer;
				
				return;
			}
			else if (m_desiredfreq == 0x0000)
			{
				m_waveState = m_waveout;
			}
			else
			{  // compute change rate
				m_changerate = (halffreq / m_desiredfreq);
			}
			
			m_sound_ptr++;
			if (!firsttime) m_sound_timer = waittime;
			firsttime = 0;
		}
		
		if (m_sound_timer) m_sound_timer--;
		
		if (m_desiredfreq==0x0000)
		{   // silence
			WaveBuffer[index] = m_silence;
		}
		else
		{
			// time to change waveform state?
			if (m_freqtimer > m_changerate)
			{  // toggle waveform, generating a square wave
				i=0;
				
				if(m_sound_ptr > 0)
					i=m_volume;
				
				if (m_waveState == m_silence-i)
					m_waveState = m_silence+i;
				else
					m_waveState = m_silence-i;
				
				m_freqtimer = 0;
			}
			else m_freqtimer++;
			
			// put wave data into buffer
			WaveBuffer[index] = m_waveState;
		}
		
		if(stereo) // Primitive stereo transformation
		{
			if(index < len)
			{
				index++;
				WaveBuffer[index] = m_waveState;
			}
		}
	}
	memcpy(waveform, WaveBuffer,len*sizeof(Uint8));
	delete[] WaveBuffer;
}

// This program reads the balance information and balances the stereo sound
void CSoundChannel::transintoStereoChannels(Uint8* waveform, unsigned int len)
{
	if(m_balance != 0) // Because, if it is not zero, there is no balance, and waves must be adapted
	{
		// -127 is only for the left speaker, while 127 is for the right speaker. 0 Is center
		
		unsigned int index;
		float f_value; // Needed for better calculation of mixing the stereo channels
		
		if(m_hq)
		{
			// balance the left channel.
			for( index = 0 ; index < len-1 ; index += 2 )
			{
				// balance here!
				f_value = (127 - (m_balance)) * ( waveform[index] - m_silence );
				f_value /= 127 ;
				if(m_silence + f_value < 255)
					waveform[index] = m_silence + f_value;
			}
			
			// balance the right channel.
			for( index = 1 ; index < len; index += 2 )
			{
				// balance here!
				f_value = (127 + (m_balance)) * ( waveform[index] - m_silence );
				f_value /= 127 ;
				if(m_silence + f_value < 255)
					waveform[index] = m_silence + f_value;
			}
		}
		else
		{
			// balance the left channel.
			for( index = 0 ; index < len-1 ; index += 2 )
			{
				// balance here!
				f_value = ( (127 - (m_balance)) * ( waveform[index] - m_silence ) ) / 127 ;
				waveform[index] = m_silence + f_value;
			}
			
			// balance the right channel.
			for( index = 1 ; index < len; index += 2 )
			{
				// balance here!
				f_value = ( (127 + (m_balance)) * ( waveform[index] - m_silence ) ) / 127 ;
				waveform[index] = m_silence + f_value;
			}
		}
	}
}

void CSoundChannel::readWaveform(Uint8* waveform, int len, Uint8 channels, int frequency)
{
    if (m_sound_playing)
    {
    	if(!m_hq) // There is no HQ sound in the buffer
     	{
			if ( m_format == AUDIO_U16 || m_format == AUDIO_S16 )
			{
				generateWaveform16( waveform, len/2, frequency, (channels==2) ? true : false );
			}
			else
			{
				generateWaveform8( waveform, len, frequency, (channels==2) ? true : false );
			}
     	}
     	else
     	{
     		stHQSound* hqsound = m_pSoundSlot[m_current_sound].getHQSoundPtr();
			
     		memcpy(waveform, hqsound->sound_buffer + m_sound_ptr, len);
         	m_sound_ptr += len;
			
         	if ((m_sound_ptr + len) > hqsound->sound_len)
         	{
         		m_sound_ptr = 0;
         		m_sound_playing = false;
         	}
     	}
		
    	if(channels == 2 && m_format == AUDIO_U8)
		{
			transintoStereoChannels(waveform, len);
		}
    }
	else
		memset(waveform,m_silence,len);
}
