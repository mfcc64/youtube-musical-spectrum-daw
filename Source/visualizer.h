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
#include "processor.h"

class YTMSWebBrowser : public juce::WebBrowserComponent {
public:
    using juce::WebBrowserComponent::WebBrowserComponent;

    bool pageAboutToLoad(const juce::String& url) override;
    void newWindowAttemptingToLoad(const juce::String& url) override;
};

class YTMSVisualizer : public juce::AudioProcessorEditor {
public:
    YTMSVisualizer(YTMSProcessor& p);
    ~YTMSVisualizer() override;
    void resized() override;
    void moved() override;

    std::optional<juce::WebBrowserComponent::Resource> get_resource(const juce::String& url);

private:
    YTMSProcessor&  processor;
    YTMSWebBrowser webview {
        juce::WebBrowserComponent::Options {}
            .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
            .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{}
                .withUserDataFolder(juce::File::getSpecialLocation (juce::File::SpecialLocationType::tempDirectory)))
            .withResourceProvider([this](const auto& url) { return get_resource(url); })
            .withKeepPageLoadedWhenBrowserIsHidden()
    };
    bool is_ready;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YTMSVisualizer)
};
