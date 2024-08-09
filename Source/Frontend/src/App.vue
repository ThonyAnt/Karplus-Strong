<template>
  <el-config-provider namespace="ep">
    <BaseHeader />
    <div class="flex main-container">
      <!-- <BaseSide /> -->
      <div w="full" py="4">
        <!-- <Logos my="4" /> -->
        <!-- <HelloWorld msg="Hello Vue 3 + Element Plus + Vite" /> -->
        <!-- <JuceSlider identifier="gainSlider" title="Gain" />
        <JuceCheckbox identifier="muteToggle"/>
        <JuceComboBox identifier="thresholdCombo"/> -->
        <!-- <Knob identifier="gainSlider"/> -->
        <Slider identifier="gainSlider"/>
        <Knob identifier="Feedback" labelTop="Feedback"/>
        <Knob identifier="Dry Gain" labelTop="Dry Gain"/>
        <Knob identifier="Wet Gain" labelTop="Wet Gain"/>
        <Knob identifier="Color" labelTop="Color"/>
        <Knob identifier="Delay Samples" labelTop="Delay Samples"/>
        <Knob identifier="Note Number" labelTop="Note Number"/>
      </div>
    </div>
  </el-config-provider>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue';
import * as Juce from "juce-framework-frontend";
import JuceSlider from "./components/JuceSlider.vue";
import JuceCheckbox from "./components/JuceCheckbox.vue";
import JuceComboBox from "./components/JuceComboBox.vue";
import Knob from "./components/Knob.vue";
import Slider from "./components/Slider.vue";
</script>

<style>
#app {
  text-align: center;
  color: var(--ep-text-color-primary);
}

.main-container {
  height: calc(100vh - var(--ep-menu-item-height) - 3px);
}

body {
    background-color: #1a1a1a;
}
</style>
