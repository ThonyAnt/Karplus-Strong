<template>
  <el-form-item :label="properties.name">
    <el-select v-model="value" @change="handleChange">
      <el-option
        v-for="(choice, i) in properties.choices"
        :key="i"
        :label="choice"
        :value="i"
      />
    </el-select>
  </el-form-item>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from "vue";
import * as Juce from "juce-framework-frontend";

const props = defineProps({
  identifier: String,
});

const comboBoxState = Juce.getComboBoxState(props.identifier);
const value = ref(comboBoxState.getChoiceIndex());
const properties = ref(comboBoxState.properties);

const handleChange = (newValue) => {
  comboBoxState.setChoiceIndex(newValue);
  value.value = newValue;
};

let valueListenerId, propertiesListenerId;

onMounted(() => {
  valueListenerId = comboBoxState.valueChangedEvent.addListener(() => {
    value.value = comboBoxState.getChoiceIndex();
  });
  propertiesListenerId = comboBoxState.propertiesChangedEvent.addListener(
    () => {
      properties.value = comboBoxState.properties;
    }
  );
});

onUnmounted(() => {
  comboBoxState.valueChangedEvent.removeListener(valueListenerId);
  comboBoxState.propertiesChangedEvent.removeListener(propertiesListenerId);
});
</script>
