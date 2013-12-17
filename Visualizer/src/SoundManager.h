#include "SimpleFMOD.h"
#include <time.h>
 
#include <queue>
 
using namespace SFMOD;
 
const float NUM_SECONDS = 0.03f;

class SoundManager
{
private:
    // FMOD
    SimpleFMOD fmod;
    Song song;
 
    // Normalization toggle and sample size
    bool enableNormalize;
    int sampleSize;
 
    // Beat detection parameters
    float beatThresholdVolume;
	float* m_bars;
    int beatThresholdBar;
    unsigned int beatSustain;
    unsigned int beatPostIgnore;
 
    int beatLastTick;
    int beatIgnoreLastTick;
 
    // List of how many ms ago the last beats were
    std::queue<int> beatTimes;
    unsigned int beatTrackCutoff;
 
    // When the music was last unpaused
    int musicStartTick;

	float m_beatStrength;
	float m_bpm;
 
public:
    SoundManager();
	~SoundManager();
	void TakeDown();
	bool Initialize();
	//void TakeDown();
    bool DrawScene();
	float GetBeatStrength();
	float GetBars(int index);
	float GetBPMEstimate();
	void Pause(bool pause);
};