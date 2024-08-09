<template>
  <div>
    <p>{{ properties.name }}: {{ calculateValue() }} {{ properties.label }}</p>
    <el-slider
      v-model="value"
      :min="0"
      :max="1"
      :step="1 / (properties.numSteps - 1)"
      :show-tooltip="false"
      @input="handleChange"
      @mousedown="mouseDown"
      @mouseup="mouseUp"
    />
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from "vue";
import * as Juce from "juce-framework-frontend";

const props = defineProps({
  identifier: String,
  title: String,
});

const sliderState = Juce.getSliderState(props.identifier);
const value = ref(sliderState.getNormalisedValue());
const properties = ref(sliderState.properties);

const handleChange = (newValue) => {
  sliderState.setNormalisedValue(newValue);
  value.value = newValue;
};

const mouseDown = () => {
  sliderState.sliderDragStarted();
};

const mouseUp = () => {
  sliderState.sliderDragEnded();
};

onMounted(() => {
  const valueListenerId = sliderState.valueChangedEvent.addListener(() => {
    value.value = sliderState.getNormalisedValue();
  });
  const propertiesListenerId = sliderState.propertiesChangedEvent.addListener(
    () => {
      properties.value = sliderState.properties;
    }
  );

  onUnmounted(() => {
    sliderState.valueChangedEvent.removeListener(valueListenerId);
    sliderState.propertiesChangedEvent.removeListener(propertiesListenerId);
  });
});

const calculateValue = () => {
  return sliderState.getScaledValue();
};
</script>

<style>
</style>
