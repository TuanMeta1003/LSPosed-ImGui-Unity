#pragma once

#include <SLES/OpenSLES.h>
#include <string>
#include <stdint.h>
#include <mutex>

using namespace std;

class AudioPlayer {
	SLObjectItf soi = nullptr;
	SLEngineItf sei = nullptr;

	SLObjectItf soi_mix = nullptr;

	SLDataLocator_URI sdl_uri;
	SLDataFormat_MIME sdf_mime;
	SLDataSource sl_data_source;

	SLDataLocator_OutputMix sdl_om;
	SLDataSink sl_data_sink;

	SLObjectItf soi_player = nullptr;
	SLPlayItf spi = nullptr;
	SLSeekItf ssi = nullptr;
	SLVolumeItf svi = nullptr;

	SLmillibel max_level;
	string current_file;
	mutex mtx;

  public:
  
	~AudioPlayer() {
		if (soi_player != nullptr)
			(*soi_player)->Destroy(soi_player);
		(*soi_mix)->Destroy(soi_mix);
		(*soi)->Destroy(soi);
	}

	AudioPlayer() {
		create();
	}

	AudioPlayer(string file) {
		create();
		setSource(file);
	}

	void create() {
		slCreateEngine(&soi, 0, NULL, 0, NULL, NULL);
		(*soi)->Realize(soi, false);
		(*soi)->GetInterface(soi, SL_IID_ENGINE, &sei);
		
		SLboolean reqf[] = { SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE };
		(*sei)->CreateOutputMix(sei, &soi_mix, 0, NULL, reqf);
		(*soi_mix)->Realize(soi_mix, false);
	}
	
	void setSource(string file) {
		mtx.lock();
		if (soi_player != nullptr) {
			(*soi_player)->Destroy(soi_player);
			soi_player = nullptr;
			spi=nullptr;
			ssi=nullptr;
			svi=nullptr;
		}
		current_file=file;
		sdl_uri.locatorType = SL_DATALOCATOR_URI;
		sdl_uri.URI = (SLchar *) file.c_str();
		sdf_mime.formatType = SL_DATAFORMAT_MIME;
		sdf_mime.mimeType = NULL;
		sdf_mime.containerType = SL_CONTAINERTYPE_UNSPECIFIED;
		sl_data_source.pLocator = &sdl_uri;
		sl_data_source.pFormat = &sdf_mime;
		
		sdl_om.locatorType = SL_DATALOCATOR_OUTPUTMIX;
		sdl_om.outputMix = soi_mix;
		sl_data_sink.pLocator = &sdl_om;
		sl_data_sink.pFormat = NULL;

		SLInterfaceID id[] = { SL_IID_SEEK, SL_IID_VOLUME };
		SLboolean req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
		
		(*sei)->CreateAudioPlayer(sei, &soi_player, &sl_data_source,&sl_data_sink, 2, id, req);
		(*soi_player)->Realize(soi_player, false);
		(*soi_player)->GetInterface(soi_player, SL_IID_PLAY, &spi);
		(*soi_player)->GetInterface(soi_player, SL_IID_SEEK, &ssi);
		(*soi_player)->GetInterface(soi_player, SL_IID_VOLUME, &svi);
		
		(*svi)->GetMaxVolumeLevel(svi, &max_level);
		mtx.unlock();
	}
	
	void play() {
		if (spi!=nullptr) {
			mtx.lock();
			(*spi)->SetPlayState(spi, SL_PLAYSTATE_PLAYING);
			mtx.unlock();
		}
	}
	
	void pause() {
		if (spi!=nullptr) {
			mtx.lock();
			(*spi)->SetPlayState(spi, SL_PLAYSTATE_PAUSED);
			mtx.unlock();
		}
	}
	
	void stop() {
		if (spi!=nullptr) {
			mtx.lock();
			(*spi)->SetPlayState(spi, SL_PLAYSTATE_STOPPED);
			mtx.unlock();
		}
	}
	
	void seek(uint32_t ms) {
		if (ssi!=nullptr) {
			mtx.lock();
			(*ssi)->SetPosition(ssi, ms, SL_SEEKMODE_FAST);
			mtx.unlock();
		}
	}
	
	uint32_t getState() {
		if (spi==nullptr) return SL_PLAYSTATE_STOPPED;
		mtx.lock();
		SLuint32 state;
		(*spi)->GetPlayState(spi,&state);
		mtx.unlock();
		return state;
	}
	
	void setVolume(int16_t level) {
		if (svi==nullptr) return;
		mtx.lock();
		(*svi)->SetVolumeLevel(svi, level > max_level ? max_level : level);
		mtx.unlock();
	}
	
	int16_t getVolume() {
		if (svi==nullptr) return -1;
		mtx.lock();
		SLmillibel ret;
		(*svi)->GetVolumeLevel(svi,&ret);
		mtx.unlock();
		return ret;
	}
	
	uint32_t getPosition() {
		if (spi==nullptr) return 0;
		mtx.lock();
		SLmillisecond position;
		(*spi)->GetPosition(spi, &position);
		mtx.unlock();
		return position;
	}
	
	uint32_t getDuration() {
		if (spi==nullptr) return 0;
		mtx.lock();
		SLmillisecond duration;
		(*spi)->GetDuration(spi, &duration);
		mtx.unlock();
		return duration;
	}
	
	string getFileName() {
		return current_file;
	}
};

