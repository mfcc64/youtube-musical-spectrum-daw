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

#include <cstdio>
#include "visualizer.h"
#include "BinaryData.h"

static juce::String size_location = juce::File::getSpecialLocation(juce::File::SpecialLocationType::tempDirectory).getFullPathName() + "/youtube-musical-spectrum-daw-size.txt";

YTMSVisualizer::YTMSVisualizer(YTMSProcessor& p) : juce::AudioProcessorEditor(&p), processor(p) {
    addAndMakeVisible(webview);
    webview.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
    setResizable(true, false);

    if (processor.public_data.bounds.isEmpty()) {
        std::FILE* fp = std::fopen(size_location.toRawUTF8(), "r");
        if (fp) {
            int w, h;
            if(std::fscanf(fp, "%d %d", &w, &h) == 2)
                processor.public_data.bounds.setSize(w, h);
            std::fclose(fp);
        }
    }

    if (processor.public_data.bounds.isEmpty())
        processor.public_data.bounds.setSize(960, 480);

    setBounds(processor.public_data.bounds);
    setResizeLimits(320, 320, 1920, 1080);
}

YTMSVisualizer::~YTMSVisualizer() { }

void YTMSVisualizer::resized() {
    webview.setBounds(getLocalBounds());
    processor.public_data.bounds = getBounds();
    std::FILE* fp = std::fopen(size_location.toRawUTF8(), "w");
    if (fp) {
        std::fprintf(fp, "%d %d", processor.public_data.bounds.getWidth(), processor.public_data.bounds.getHeight());
        std::fclose(fp);
    }
}

void YTMSVisualizer::moved() {
    processor.public_data.bounds = getBounds();
}

std::optional<juce::WebBrowserComponent::Resource> YTMSVisualizer::get_resource(const juce::String& url) {
    using namespace BinaryData;

    static struct {
        const char* url;
        const char* data;
        int length;
        const char* mime_type;
    } url_list[] = {
        { "/", index_html, index_htmlSize, "text/html" },
        { "/modules/@mfcc64/ytms/script.mjs", script_mjs, script_mjsSize, "text/javascript" },
        { "/modules/@mfcc64/ytms-assets@1.0.0/axis-hz-1920x32.png", axishz1920x32_png, axishz1920x32_pngSize, "image/png" },
        { "/modules/@mfcc64/ytms-assets@1.0.0/icon-16.png", icon16_png, icon16_pngSize, "image/png" },
        { "/modules/@mfcc64/ytms-assets@1.0.0/icon-24.png", icon24_png, icon24_pngSize, "image/png" },
        { "/modules/@mfcc64/ytms-assets@1.0.0/icon-32.png", icon32_png, icon32_pngSize, "image/png" },
        { "/modules/@mfcc64/ytms-assets@1.0.0/icon-48.png", icon48_png, icon48_pngSize, "image/png" },
        { "/modules/@mfcc64/ytms-assets@1.0.0/icon-64.png", icon64_png, icon64_pngSize, "image/png" },
        { "/modules/@mfcc64/ytms-assets@1.0.0/icon-128.png", icon128_png, icon128_pngSize, "image/png" },
        { "/modules/showcqt-element@2/showcqt-element.mjs", showcqtelement_mjs, showcqtelement_mjsSize, "text/javascript" },
        { "/modules/showcqt-element@2.2.1/audio-worklet.mjs", audioworklet_mjs, audioworklet_mjsSize, "text/javascript" },
        { "/modules/showcqt-element@2.2.1/axis-1920x32.png", axis1920x32_png, axis1920x32_pngSize, "image/png" },
        { "/modules/showcqt-element@2.2.1/extern-cdn.mjs", externcdn_mjs, externcdn_mjsSize, "text/javascript" },
        { "/modules/showcqt-element@2.2.1/extern.mjs", extern_mjs, extern_mjsSize, "text/javascript" },
        { "/modules/showcqt-element@2.2.1/showcqt-element-main.mjs", showcqtelementmain_mjs, showcqtelementmain_mjsSize, "text/javascript" },
        { "/modules/showcqt@2/showcqt.mjs", showcqt_mjs, showcqt_mjsSize, "text/javascript" },
        { "/modules/showcqt@2.2.2/showcqt-main.mjs", showcqtmain_mjs, showcqtmain_mjsSize, "text/javascript" },
        { "/modules/showcqt@2.2.2/showcqt-simd.wasm", showcqtsimd_wasm, showcqtsimd_wasmSize, "application/wasm" },
        { "/modules/showcqt@2.2.2/showcqt.wasm", showcqt_wasm, showcqt_wasmSize, "application/wasm" }
    };

    if (url == "/pcm-data") {
        int read_index = processor.public_data.read_index;
        int write_index = processor.public_data.write_index.load();
        int nb_samples = (write_index - read_index) & processor.BUFFER_MASK;

        if (!nb_samples)
            return juce::WebBrowserComponent::Resource { std::vector<std::byte>(1), "application/octet-stream" };

        std::vector<std::byte> output(nb_samples * 2 * sizeof(float));
        const float* buffer = processor.public_data.buffer;
        float* data = (float*) output.data();

        for (int x = 0; x < nb_samples; x++) {
            int i = (read_index + x) & processor.BUFFER_MASK;
            data[x] = buffer[2*i];
            data[nb_samples+x] = buffer[2*i+1];
        }

        processor.public_data.read_index = write_index;
        return juce::WebBrowserComponent::Resource { std::move(output), "application/octet-stream" };
    }

    if (url == "/sample-rate") {
        auto rate_ptr = (std::byte*) &processor.public_data.sample_rate;

        return juce::WebBrowserComponent::Resource {
            std::vector<std::byte>(rate_ptr, rate_ptr + 4),
            "application/octet-stream"
        };
    }

    for (int k = 0; k < sizeof(url_list) / sizeof(url_list[0]); k++) {
        if (url == url_list[k].url) {
            auto src = (const std::byte*) url_list[k].data;
            return juce::WebBrowserComponent::Resource {
                std::vector<std::byte>(src, src + url_list[k].length),
                url_list[k].mime_type
            };
        }
    }

    return std::nullopt;
}

bool YTMSWebBrowser::pageAboutToLoad(const juce::String& url) {
    return url == getResourceProviderRoot();
}

void YTMSWebBrowser::newWindowAttemptingToLoad(const juce::String& url) {
    juce::Process::openDocument(url, "");
}
