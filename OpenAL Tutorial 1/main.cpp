/**	Wrote this for reference when using OpenAL in the future
	Used the following to understand what I was doing and write this:

	https://github.com/benwad/Spong
	http://soundfile.sapp.org/doc/WaveFormat/
	http://enigma-dev.org/forums/index.php?topic=730.0

	//Official OpenAL Porgrammer's Guide
	https://openal.org/documentation/OpenAL_Programmers_Guide.pdf#[{%22num%22%3A340%2C%22gen%22%3A0}%2C{%22name%22%3A%22XYZ%22}%2C0%2C792%2Cnull


	**NOTE: I do not own the song Smooth Criminal (Re-interpretation) by Haywyre, nor do I claim to be the creator of said song.
	I am simply using it for the purpose of this demo.
*/

#include <al.h>
#include <alc.h>
#include <iostream>
#include <fstream>

//return short give a buffer of 2 bytes
unsigned short BytesToShort(char* buffer);

//return int give a buffer of 4 bytes
unsigned int BytesToInt(char* buffer);

int main() {

	//creating a device pointer from the default system device
	ALCdevice *device = alcOpenDevice(NULL);

	const ALCchar* devices = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	std::cout << devices << std::endl;

	//creating and establishing OpenAL context
	ALCcontext *context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	//defining the listener (where it is, how it's moving, and its orientation)
	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);

	//defining a source for the sound in a similar manner
	ALuint source;
	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, 1);				//how fast the audio is played (at 100% speed)
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, AL_FALSE);	//audio doesn't loop when done

	//generating buffer for sound data
	ALuint buffer;
	alGenBuffers(1, &buffer);
	
	//loading sound file
	std::fstream file("Haywyre - Smooth Criminal (Re-interpretation).wav", std::fstream::in | std::fstream::binary);
	char xbuffer[5];
	xbuffer[4] = '\0';

	file.read(xbuffer, 4);	//reading chunck ID
	if (strcmp(xbuffer, "RIFF") != 0) std::cout << "Not a wav file!" << std::endl;
	else  std::cout << "Chunck ID: " << xbuffer << std::endl;

	file.read(xbuffer, 4);	//reading chunk size;
	unsigned int chunkSize = BytesToInt(xbuffer);
	std::cout << "Chunk Size: " << chunkSize << std::endl;

	file.read(xbuffer, 4);	//reading format
	if (strcmp(xbuffer, "WAVE") != 0) std::cout << "Not a wav file!" << std::endl;
	else  std::cout << "Format: " << xbuffer << std::endl;

	file.read(xbuffer, 4);	//reading subchunk1ID
	if (strcmp(xbuffer, "fmt ") != 0) std::cout << "Invalid WAV file!" << std::endl;
	else std::cout << "Subchunck ID: " << xbuffer << std::endl;

	file.read(xbuffer, 4);	//reading subchunk size;
	unsigned int subChunkSize = BytesToInt(xbuffer);
	std::cout << "Subchunck Size: " << subChunkSize << std::endl;

	file.read(xbuffer, 2);		//reading audio format
	unsigned short audioFormat = BytesToShort(xbuffer);
	std::cout << "Audio Format: " << audioFormat << std::endl;
	
	file.read(xbuffer, 2);		//reading number of channels
	unsigned short channels = BytesToShort(xbuffer);
	std::cout << "Channels: " << channels << std::endl;

	file.read(xbuffer, 4);		//read sample rate
	unsigned short sampleRate = BytesToInt(xbuffer);
	std::cout << "Sample Rate: " << sampleRate << std::endl;

	file.read(xbuffer, 4);		//read byte rate
	unsigned int byteRate = BytesToInt(xbuffer);
	std::cout << "Byte Rate: " << byteRate << std::endl;

	file.read(xbuffer, 2);	//reading block align;
	unsigned short blockAlign = BytesToShort(xbuffer);
	std::cout << "Block Align: " << blockAlign << std::endl;
	
	file.read(xbuffer, 2);		//reading bits per sample
	unsigned short bitsPerSample = BytesToShort(xbuffer);
	std::cout << "Bits per Sample: " << bitsPerSample << std::endl;

	file.read(xbuffer, 4);	//reading subchunk2ID
	if (strcmp(xbuffer, "data") != 0) std::cout << "Invalid WAV file!" << std::endl;
	else std::cout << "Subchunck 2 ID: " << xbuffer << std::endl;

	file.read(xbuffer, 4);	//reading data buffer size
	unsigned int bufferSize = BytesToInt(xbuffer);
	std::cout << "Data Buffer Size: " << bufferSize << std::endl;

	//reading actual audio data from file
	unsigned char* bufferData = (unsigned char*)malloc(bufferSize);
	file.read(reinterpret_cast<char*>(bufferData), bufferSize);

	file.close();

	//choosing format based on bits per sample and number of channels
	ALenum format;
	if (channels == 1) {
		if(bitsPerSample == 8) format = AL_FORMAT_MONO8;
		else format = AL_FORMAT_MONO16;
	}
	else {
		if (bitsPerSample == 8) format = AL_FORMAT_STEREO8;
		else format = AL_FORMAT_STEREO16;
	}

	//Calculating duration of file
	float duration = ((float)bufferSize) / byteRate;
	std::cout << "Duration is: " << duration << std::endl;

	//loading audio data to the buffer and freeing it from memory
	alBufferData(buffer, format, bufferData, bufferSize, sampleRate);
	free(bufferData);

	//setting the buffer of the source
	alSourcei(source, AL_BUFFER, buffer);
	
	//Playing audio
	alSourcePlay(source);
	std::cout << "Playing" << std::endl;
	fgetc(stdin);

	//freeing allocated memory
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	alcDestroyContext(context);
	alcCloseDevice(device);

	return 0;
}

//return short give a buffer of 2 bytes
unsigned short BytesToShort(char* buffer) {
	unsigned short out = 0, byte0, byte1;
	
	byte0 = (unsigned short)(buffer[0]) & 0x00ff;
	byte1 = (unsigned short)(buffer[1] << 8) & 0xff00;

	out += byte0 | byte1;
	return out;
}
//return int give a buffer of 4 bytes
unsigned int BytesToInt(char* buffer) {
	unsigned int out = 0, byte0, byte1, byte2, byte3;

	byte0 = (unsigned int)(buffer[0])		& 0x000000ff;
	byte1 = (unsigned int)(buffer[1] << 8)	& 0x0000ff00;
	byte2 = (unsigned int)(buffer[2] << 16) & 0x00ff0000;
	byte3 = (unsigned int)(buffer[3] << 24) & 0xff000000;

	out += byte0 | byte1 | byte2 | byte3;
	return out;
}