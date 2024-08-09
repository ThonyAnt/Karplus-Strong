<template>
  <div class="slider-container" @mousedown="startDrag" @mousemove="onDrag" @mouseup="endDrag" @mouseleave="endDrag">
    <svg width="100%" height="40">
      <defs>
        <filter id="glow" x="-50%" y="-50%" width="200%" height="200%">
          <feGaussianBlur stdDeviation="3.5" result="coloredBlur" />
          <feMerge>
            <feMergeNode in="coloredBlur" />
            <feMergeNode in="SourceGraphic" />
          </feMerge>
        </filter>
      </defs>
      <line x1="10" y1="20" x2="calc(100% - 10px)" y2="20" stroke="#555" stroke-width="6" />
      <rect :x="10" y="17" :width="position - 10" height="6" fill="cyan" filter="url(#glow)" />
      <circle :cx="position" cy="20" r="10" fill="#fff" />
    </svg>
  </div>
</template>

<script setup>
import { ref, computed, watch } from "vue";

const props = defineProps({
  min: {
    type: Number,
    default: 0,
  },
  max: {
    type: Number,
    default: 100,
  },
  modelValue: {
    type: Number,
    default: 50,
  },
});

const emit = defineEmits(["update:modelValue"]);

const position = ref(0);
const dragging = ref(false);

const updatePosition = (event) => {
  const slider = event.currentTarget;
  const rect = slider.getBoundingClientRect();
  const offsetX = event.clientX - rect.left;
  const newPosition = Math.max(10, Math.min(offsetX, rect.width - 10));
  position.value = newPosition;
  const value = ((newPosition - 10) / (rect.width - 20)) * (props.max - props.min) + props.min;
  emit("update:modelValue", Math.round(value));
};

const startDrag = (event) => {
  dragging.value = true;
  updatePosition(event);
};

const onDrag = (event) => {
  if (dragging.value) {
    updatePosition(event);
  }
};

const endDrag = () => {
  dragging.value = false;
};

const positionFromValue = computed(() => {
  const sliderWidth = 100; // Assuming the slider width is 100%
  return ((props.modelValue - props.min) / (props.max - props.min)) * (sliderWidth - 20) + 10;
});

watch(
  () => props.modelValue,
  (newValue) => {
    position.value = positionFromValue.value;
  }
);

position.value = positionFromValue.value;
</script>

<style scoped>
.slider-container {
  width: 100%;
  height: 40px;
  background-color: transparent; /* 设置为透明 */
  display: flex;
  justify-content: center;
  align-items: center;
  user-select: none;
}
</style>
