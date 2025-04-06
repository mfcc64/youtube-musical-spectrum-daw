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

#pragma once

#include <JuceHeader.h>

class YTMSProcessor : public juce::AudioProcessor {
public:
    YTMSProcessor();
    ~YTMSProcessor() override;

    void prepareToPlay(double rate, int block_size) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& name) override;

    void getStateInformation(juce::MemoryBlock& data) override;
    void setStateInformation(const void* data, int size) override;

    static const int BUFFER_SIZE = 65536;
    static const int BUFFER_MASK = BUFFER_SIZE - 1;
    struct {
        juce::Rectangle<int> bounds { 64, 64, 960, 480 };
        std::int32_t sample_rate;
        int read_index;
        float buffer[BUFFER_SIZE * 2];
        int _pad_0[256];
        std::atomic<int> write_index;
        int _pad_1[256];
    } public_data;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YTMSProcessor)
};
