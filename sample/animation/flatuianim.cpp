// Copyright 2016 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cassert>
#include "flatui/flatui.h"
#include "flatui/flatui_common.h"
#include "fplbase/input.h"
#include "fplbase/renderer.h"
#include "fplbase/utilities.h"

#include "motive/engine.h"
#include "motive/init.h"

using flatui::Run;
using flatui::EndGroup;
using flatui::Label;
using flatui::Margin;
using flatui::SetVirtualResolution;
using flatui::Slider;
using flatui::StartGroup;
using mathfu::vec2;
using mathfu::vec2i;
using mathfu::vec4;
using mathfu::kZeros4f;
using motive::Motivator4f;
using motive::MotiveEngine;
using motive::MotiveTime;
using motive::SplineInit;
using motive::Tar4f;

static const MotiveTime kTimeToFade = 1000;

static const vec4 kBackgroundColors[] = {
    vec4(0.5f, 0.5f, 0.5f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f),
    vec4(0.0f, 0.0f, 1.0f, 1.0f), vec4(0.0f, 1.0f, 1.0f, 1.0f),
};

extern "C" int FPL_main(int /*argc*/, char** argv) {
  MotiveEngine motive_engine;
  SplineInit::Register();

  size_t color_idx = 0;
  Motivator4f background_color;
  background_color.InitializeWithTarget(
      SplineInit(), &motive_engine,
      Tar4f::Current(kBackgroundColors[color_idx]));

  fplbase::Renderer renderer;
  renderer.Initialize(vec2i(800, 600), "FlatUI sample");

  fplbase::InputSystem input;
  input.Initialize();

  // Set the local directory to the assets folder for this sample.
  bool result = fplbase::ChangeToUpstreamDir(argv[0], "sample/assets");
  assert(result);

  fplbase::AssetManager assetman(renderer);

  flatui::FontManager fontman;
  fontman.SetRenderer(renderer);
  // Open OpenType font.
  fontman.Open("fonts/NotoSansCJKjp-Bold.otf");

  while (!(input.exit_requested() ||
           input.GetButton(fplbase::FPLK_AC_BACK).went_down())) {
    input.AdvanceFrame(&renderer.window_size());
    renderer.AdvanceFrame(input.minimized(), input.Time());
    renderer.ClearFrameBuffer(background_color.Value());
    motive_engine.AdvanceFrame(
        static_cast<MotiveTime>(input.DeltaTime() * 1000.0f));

    // Show test GUI using flatui API.
    // In this sample, it shows basic UI elements of a label and an image.
    // Define flatui block. Note that the block is executed multiple times,
    // One for a layout pass and another for a rendering pass.
    Run(assetman, fontman, input, [&]() {
      // Set a virtual resolution all coordinates will use. 1000 is now the
      // size of the smallest dimension (Y in landscape mode).
      SetVirtualResolution(1000);

      // Start our root group.
      StartGroup(flatui::kLayoutVerticalLeft);

      // Position group in the center of the screen.
      PositionGroup(flatui::kAlignCenter, flatui::kAlignCenter,
                    mathfu::kZeros2f);

      // Set the color of the button.
      flatui::ColorBackground(vec4(225.0f, 128.0f, 0.0f, 0.1f));
      flatui::SetHoverClickColor(vec4(255.0f, 0.0f, 255.0f, 0.8f),
                                 vec4(255.0f, 228.0f, 196.0f, 0.5f));

      // Create the button and check for input event.
      if (TextButton("Animate", 50, Margin(10)) & flatui::kEventWentUp) {
        color_idx = (color_idx + 1) % FPL_ARRAYSIZE(kBackgroundColors);
        background_color.SetTarget(
            Tar4f::Target(kBackgroundColors[color_idx], kZeros4f, kTimeToFade));
      }

      EndGroup();
    });
  }

  return 0;
}
