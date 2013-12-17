// FMOD Frequency Analysis demo
// Written by Katy Coe (c) 2013
// No unauthorized copying or distribution
// www.djkaty.com
#include "SoundManager.h"

 
// Initialize application
SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

bool SoundManager::Initialize() {
	this->m_bpm = 0.0f;
	song = fmod.LoadSong("audio/Song5.mp3", FMOD_SOFTWARE);
    song.Start(true);
 
    // Load song
    enableNormalize = true;
    sampleSize = 64;
 
    // Set beat detection parameters
    beatThresholdVolume = 0.25f;
    beatThresholdBar = 0;
    beatSustain = 200;
    beatPostIgnore = 20;

	//how long to look for bpm
    beatTrackCutoff = 5000;
 
    beatLastTick = 0;
    beatIgnoreLastTick = 0;
 
    musicStartTick = 0;
	
	m_beatStrength = 0.0f;
	m_bars = new float[64 * 2];
	return true;
}
 
void SoundManager::Pause(bool pause)
{
	song.SetPaused(pause);
}

float SoundManager::GetBeatStrength() {
	return m_beatStrength;
}

float SoundManager::GetBars(int index) {
	return m_bars[index];
}
 
// Per-frame code
bool SoundManager::DrawScene()
{
	bool beat = false;
    // Update FMOD
	song.SetPaused(false);
    fmod.Update();
	enableNormalize = false;
	sampleSize = 64 * 2;
 
    // Frequency analysis
    float *specLeft, *specRight;// *spec;
    //spec = new float[sampleSize];
    specLeft = new float[sampleSize];
    specRight = new float[sampleSize];
 
    // Get average spectrum for left and right stereo channels
    song.GetChannel()->getSpectrum(specLeft, sampleSize, 0, FMOD_DSP_FFT_WINDOW_RECT);
    song.GetChannel()->getSpectrum(specRight, sampleSize, 1, FMOD_DSP_FFT_WINDOW_RECT);
	
    for (int i = 0; i < sampleSize; i++) {
		m_bars[i] = (specLeft[i] + specRight[i]) / 2;
	}
    // Find max volume
    auto maxIterator = std::max_element(&m_bars[0], &m_bars[sampleSize]);
    float maxVol = *maxIterator;
 
    // Find frequency range of each array item
    float hzRange = (44100 / 2) / static_cast<float>(sampleSize);
 
    // Detect beat if normalization disabled
    if (!enableNormalize)
    {
        if (m_bars[beatThresholdBar] >= beatThresholdVolume && beatLastTick == 0 && beatIgnoreLastTick == 0)
        {
			m_beatStrength = m_bars[beatThresholdBar];
            beatLastTick = GetTickCount();
            beatTimes.push(beatLastTick);
 
            while(GetTickCount() - beatTimes.front() > beatTrackCutoff)
            {
                beatTimes.pop();
                if (beatTimes.size() == 0)
                    break;
            }
        }
 
        if (GetTickCount() - beatLastTick < beatSustain) {
			beat = true;
			//printf("BEAT\n");
		} else if (beatIgnoreLastTick == 0 && beatLastTick != 0) {
            beatLastTick = 0;
            beatIgnoreLastTick = GetTickCount();
        }
 
        if (GetTickCount() - beatIgnoreLastTick >= beatPostIgnore)
            beatIgnoreLastTick = 0;
    }
 
    // Predict BPM
    float msPerBeat, bpmEstimate;
 
    if (beatTimes.size() >= 2)
    {
        msPerBeat = (beatTimes.back() - beatTimes.front()) / static_cast<float>(beatTimes.size() - 1);
        bpmEstimate = 60000 / msPerBeat;
    }
    else
        bpmEstimate = 0;
	
	//printf("BPM Estimate = %f\n", bpmEstimate);
	m_bpm = bpmEstimate;

 
    // Clean up
    delete [] specLeft;
    delete [] specRight;
	if (beat) {
		return true;
	} else {
		return false;
	}
}
 
float SoundManager::GetBPMEstimate() 
{
	//m_bpm = 100.0f * GetBars(0);
	//if (m_bpm < 50.0f) m_bpm = 50.0f;
	return m_bpm;
}

void SoundManager::TakeDown()
{
	song.Stop();
	fmod.~SimpleFMOD();
}
