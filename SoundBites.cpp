#include <wave/file.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>

/// Fraction of the maximum absolute difference from the average that triggers
/// a sound event.
static float g_fracSound = 0.38f;

/// Fraction of the maximum absolute difference from the average that is
/// declared to be silent again.
static float g_fracSilence = 0.01f;

/// How many consecutive samples have to be silence before we declare the end
/// (or beginning) of a sound.
static int g_silenceSamples = 5;

void Usage(std::string name)
{
  std::cerr << "Usage: " << name << " [-v] inFile outFileBase" << std::endl;
  std::cerr << "       -v: Verbose, print additional info" << std::endl;
  std::cerr << "       inFile: Name of the input WAV file to read from" << std::endl;
  std::cerr << "       outFileName: Base name of output file, #####.wav added" << std::endl;
  exit(-1);
}

int main(int argc, const char *argv[])
{
  // Parse the command line
  bool verbose = false;
  std::string inFileName, outFileBaseName;
  size_t realParams = 0;
  for (int i = 1; i < argc; i++) {
    if (std::string("-help") == argv[i]) {
      Usage(argv[0]);
    } else if (std::string("-v") == argv[i]) {
      verbose = true;
    } else if (argv[i][0] == '-') {
      Usage(argv[0]);
    } else switch (++realParams) {
    case 1:
      inFileName = argv[i];
      break;
    case 2:
      outFileBaseName = argv[i];
      break;
    default:
      Usage(argv[0]);
    }
  }
  if (realParams != 2) {
    Usage(argv[0]);
  }

  // read file's content
  wave::File read_file;
  wave::Error err = read_file.Open(inFileName.c_str(), wave::kIn);
  if (err) {
    std::cerr << "Something went wrong in in open" << std::endl;
    return 1;
  }
  std::vector<float> content;
  err = read_file.Read(&content);
  if (err) {
    std::cerr << "Something went wrong in read" << std::endl;
    return 2;
  }

  // Find out how many channels we have in the file.
  uint16_t numChannels = read_file.channel_number();
  if (verbose) { std::cout << "Found " << numChannels << " channels" << std::endl; }

  // Find the average value for each channel, along with the
  // maximum value of the largest deviation from that average.
  std::vector<float> means;
  std::vector<float> mags;
  for (uint16_t c = 0; c < numChannels; c++) {
    float sum = 0;
    for (size_t i = 0; i < content.size() / numChannels; i++) {
      sum += content[numChannels * i + c];
    }
    means.push_back(sum / (content.size() / numChannels));
    float maxDiff = 0;
    for (size_t i = 0; i < content.size() / numChannels; i++) {
      float diff = abs(content[numChannels * i + c] - means[c]);
      if (diff > maxDiff) { maxDiff = diff; }
    }
    mags.push_back(maxDiff);
  }
  if (verbose) {
    std::cout << "Means: "<< std::endl;
    for (size_t i = 0; i < means.size(); i++) {
      std::cout << "  " << means[i] << std::endl;
    }
    std::cout << "Maximum absolute value differences: " << std::endl;
    for (size_t i = 0; i < mags.size(); i++) {
      std::cout << "  " << mags[i] << std::endl;
    }
  }

  // Run through the file.  Each time we come across a sample whose magnitude
  // is further than the threshold from the mean on any channel, find the sound
  // chunk that includes it and write it to a new file.
  int whichOutput = 0;
  size_t i = 0;
  while (i < content.size() / numChannels) {
    // Seek a value above threshold.  If we don't have one, we keep
    // advancing.
    bool foundSound = false;
    for (uint16_t c = 0; c < numChannels; c++) {
      float diff = abs(content[numChannels * i + c]);
      if (diff - means[c] >= g_fracSound * mags[c]) {
        foundSound = true;
      }
    }
    if (!foundSound) {
      i++;
      continue;
    }

    // Extract the section that includes this sound. Go backwards and forwards
    // either to the file boundary or until we get enough consistent silence
    // values.
    size_t start;
    int zeroCount = 0;
    for (start = i; (start > 0) && (zeroCount < g_silenceSamples); start--) {
      bool foundSilence = true;
      for (uint16_t c = 0; c < numChannels; c++) {
        float diff = abs(content[numChannels * start + c] - means[c]);
        if (diff > g_fracSilence * mags[c]) {
          foundSilence = false;
        }
      }
      if (foundSilence) { zeroCount++; }
      else { zeroCount = 0; }
    }
    size_t end;
    zeroCount = 0;
    for (end = i; (end < content.size() / numChannels - 1) && (zeroCount < g_silenceSamples); end++) {
      bool foundSilence = true;
      for (uint16_t c = 0; c < numChannels; c++) {
        float diff = abs(content[numChannels * end + c] - means[c]);
        if (diff > g_fracSilence * mags[c]) {
          foundSilence = false;
        }
      }
      if (foundSilence) { zeroCount++; }
      else { zeroCount = 0; }
    }
    if (verbose) {
      std::cout << "Found sound " << whichOutput << " from " << start
        << " to " << end << std::endl;
    }

    // Extract the content of just this sound into a new vector.
    std::vector<float> snippet;
    for (size_t s = start; s <= end; s++) {
      for (uint16_t c = 0; c < numChannels; c++) {
        snippet.push_back(content[numChannels * s + c]);
      }
    }

    // Write the section to a file whose name depends on the index.
    wave::File write_file;
    char number[10];
    sprintf(number, "%05d", whichOutput);
    std::string outFileName = outFileBaseName + number + ".wav";
    write_file.Open(outFileName.c_str(), wave::kOut);
    if (err) {
      std::cerr << "Something went wrong in out open" << std::endl;
      return 3;
    }
    write_file.set_sample_rate(read_file.sample_rate());
    write_file.set_bits_per_sample(read_file.bits_per_sample());
    write_file.set_channel_number(numChannels);

    err = write_file.Write(snippet);
    if (err) {
      std::cerr << "Something went wrong in write" << std::endl;
      return 4;
    }

    // Skip past this section, which means one past the end of the sound.
    i = end + 1;

    // Increment the output number for the next file.
    whichOutput++;
  }

  return 0;
}