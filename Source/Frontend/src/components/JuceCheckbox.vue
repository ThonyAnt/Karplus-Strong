<template>
  <div>
    <el-checkbox v-model="value" @change="handleChange">{{ properties.name }}</el-checkbox>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from "vue";
import * as Juce from "juce-framework-frontend";

const props = defineProps({
  identifier: String,
});

let checkboxState = Juce.getToggleState(props.identifier);
const value = ref(checkboxState.getValue());
const properties = ref(checkboxState.properties);

const handleChange = (checked) => {
  checkboxState.setValue(checked);
  value.value = checked;
};

onMounted(() => {
  value.value = checkboxState.getValue();
  properties.value = checkboxState.properties;

  checkboxState.valueChangedEvent.addListener(() => {
    value.value = checkboxState.getValue();
  });

  checkboxState.propertiesChangedEvent.addListener(() => {
    properties.value = checkboxState.properties;
  });
});

onUnmounted(() => {
  checkboxState.valueChangedEvent.removeListener();
  checkboxState.propertiesChangedEvent.removeListener();
});
</script>
