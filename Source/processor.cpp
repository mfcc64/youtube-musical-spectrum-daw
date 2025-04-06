/*
 * Copyright (c) 2025 Muhammad Faiz <mfcc64@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "processor.h"
#include "visualizer.h"

YTMSProcessor::YTMSProcessor() :
    AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ) { }

YTMSProcessor::~YTMSProcessor() { }
const juce::String YTMSProcessor::getName() const { return JucePlugin_Name; }
bool YTMSProcessor::acceptsMidi() const { return false; }
bool YTMSProcessor::producesMidi() const { return false; }
bool YTMSProcessor::isMidiEffect() const { return false; }
double YTMSProcessor::getTailLengthSeconds() const { return 0; }
int YTMSProcessor::getNumPrograms() { return 1; }
int YTMSProcessor::getCurrentProgram() { return 0; }
void YTMSProcessor::setCurrentProgram(int) { }
const juce::String YTMSProcessor::getProgramName(int index) { return {}; }
void YTMSProcessor::changeProgramName(int, const juce::String&) { }

void YTMSProcessor::prepareToPlay(double rate, int) {
    public_data.sample_rate = (std::int32_t) rate;
}

void YTMSProcessor::releaseResources() { }

bool YTMSProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void YTMSProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
    const float* data[2] = {
        buffer.getReadPointer(0),
        buffer.getReadPointer(1)
    };

    int start = public_data.write_index.load();
    int nb_samples = buffer.getNumSamples();

    for (int x = 0; x < nb_samples; x++) {
        int i = (start + x) & BUFFER_MASK;
        public_data.buffer[2*i+0] = data[0][x];
        public_data.buffer[2*i+1] = data[1][x];
    }

    public_data.write_index.store((start + nb_samples) & BUFFER_MASK);
}

bool YTMSProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* YTMSProcessor::createEditor() {
    return new YTMSVisualizer(*this);
}

void YTMSProcessor::getStateInformation(juce::MemoryBlock& data) {
    //
}

void YTMSProcessor::setStateInformation(const void* data, int size) {
    //
}
