<template>
  <div class="knob-container" @mousedown="startDrag" :style="{ width: size + 'px', height: size + 'px' }">
    <div class="label-top">{{ labelTop }}</div>
    <svg :width="size" :height="size" :viewBox="'0 0 ' + size + ' ' + size">
      <path :d="circlePath" stroke="#333" :stroke-width="strokeWidth" fill="none" />
      <path :d="arcPath" stroke="white" :stroke-width="strokeWidth" fill="none" stroke-linecap="round" />
    </svg>
    <div class="value-display" @dblclick="enableEditing">
      <span v-if="!editing">{{ formattedValue() }}</span>
      <input v-else type="number" v-model.number="inputValue" @blur="confirmValue" @keyup.enter="confirmValue" />
    </div>
  </div>
</template>

<script setup>
import { ref, computed, watch, onMounted, onBeforeUnmount } from "vue";
import * as Juce from "juce-framework-frontend";

const props = defineProps({
  identifier: String,
  minValue: {
    type: Number,
    default: 0,
  },
  maxValue: {
    type: Number,
    default: 1,
  },
  currentValue: {
    type: Number,
    default: 0,
  },
  size: {
    type: Number,
    default: 200,
  },
  strokeWidth: {
    type: Number,
    default: 5,
  },
  labelTop: {
    type: String,
    default: "TEST",
  },
});

const sliderState = Juce.getSliderState(props.identifier);
// const sliderValue = ref(props.currentValue);
const sliderValue = ref(sliderState.getNormalisedValue());
const properties = ref(sliderState.properties);

const dragging = ref(false);
const startY = ref(0);
const editing = ref(false);
const inputValue = ref(sliderValue.value);

watch(
  () => props.currentValue,
  (newValue) => {
    sliderValue.value = newValue;
  }
);

watch(
  sliderValue,
  (newValue) => {
    console.log(newValue);
    sliderState.setNormalisedValue(newValue);
  }
);

const polarToCartesian = (radius, angleInRadians) => {
  return {
    x: radius * Math.cos(angleInRadians) + props.size / 2,
    y: radius * Math.sin(angleInRadians) + props.size / 2,
  };
};

const circlePath = computed(() => {
  const radius = props.size / 2 - props.strokeWidth;
  const startAngle = -225;
  const endAngle = 45;

  const start = polarToCartesian(radius, (startAngle * Math.PI) / 180);
  const end = polarToCartesian(radius, (endAngle * Math.PI) / 180);

  return ["M", start.x, start.y, "A", radius, radius, 0, 1, 1, end.x, end.y].join(" ");
});

const arcPath = computed(() => {
  if (sliderValue.value === props.minValue) {
    return ""; // 当值为0时，不绘制路径
  }
  const startAngle = -225;
  const endAngle = startAngle + ((sliderValue.value - props.minValue) * 270) / (props.maxValue - props.minValue);
  const largeArcFlag = endAngle - startAngle <= 180 ? 0 : 1;
  const radius = props.size / 2 - props.strokeWidth;
  const start = polarToCartesian(radius, (startAngle * Math.PI) / 180);
  const end = polarToCartesian(radius, (endAngle * Math.PI) / 180);
  return ["M", start.x, start.y, "A", radius, radius, 0, largeArcFlag, 1, end.x, end.y].join(" ");
});

const formattedValue = () => {
  // return sliderValue.value.toFixed(2);
  console.log(sliderState.getScaledValue());
  return sliderState.getScaledValue().toFixed(2);
};

// 这个函数在用户按下鼠标按钮时触发。
// 它将 dragging 状态设置为 true，并记录初始的 Y 坐标。
// 它还添加了 mousemove 和 mouseup 事件监听器，以便在拖动过程中更新值，并在拖动结束时清理事件监听器。
const startDrag = (event) => {
  dragging.value = true;
  startY.value = event.clientY;
  window.addEventListener("mousemove", onDrag);
  window.addEventListener("mouseup", stopDrag);

  sliderState.sliderDragStarted();
};

// 这个函数在用户拖动鼠标时触发。
// 它计算鼠标移动的距离 (deltaY)，并根据这个距离和滑块的大小 (size) 计算新的值。
// 它确保新的值在 minValue 和 maxValue 之间。
const onDrag = (event) => {
  if (!dragging.value) return;
  const deltaY = startY.value - event.clientY;
  const valueRange = props.maxValue - props.minValue;
  const step = valueRange / props.size;
  sliderValue.value = Math.min(props.maxValue, Math.max(props.minValue, sliderValue.value + deltaY * step));
  startY.value = event.clientY; 
};

// 这个函数在用户释放鼠标按钮时触发。
// 它将 dragging 状态设置为 false，并移除 mousemove 和 mouseup 事件监听器。
const stopDrag = () => {
  dragging.value = false;
  window.removeEventListener("mousemove", onDrag);
  window.removeEventListener("mouseup", stopDrag);

  sliderState.sliderDragEnded();
};

const enableEditing = () => {
  editing.value = true;
  inputValue.value = sliderValue.value;
};

const confirmValue = () => {
  if (inputValue.value >= props.minValue && inputValue.value <= props.maxValue) {
    sliderValue.value = inputValue.value;
  } else if (inputValue.value < props.minValue) {
    sliderValue.value = props.minValue;
  } else if (inputValue.value > props.maxValue) {
    sliderValue.value = props.maxValue;
  }
  editing.value = false;
};

onMounted(() => {
  window.addEventListener("mouseup", stopDrag);

  // const valueListenerId = sliderState.valueChangedEvent.addListener(() => {
  //   value.value = sliderState.getNormalisedValue();
  // });
  // const propertiesListenerId = sliderState.propertiesChangedEvent.addListener(
  //   () => {
  //     properties.value = sliderState.properties;
  //   }
  // );

  // onUnmounted(() => {
  //   sliderState.valueChangedEvent.removeListener(valueListenerId);
  //   sliderState.propertiesChangedEvent.removeListener(propertiesListenerId);
  // });
});

onBeforeUnmount(() => {
  window.removeEventListener("mouseup", stopDrag);
});
</script>

<style scoped>
.knob-container {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  user-select: none;
  position: relative;
}

.label-top {
  position: absolute;
  top: -30px; /* 调整这个值来改变顶部标签的位置 */
  font-size: 1.2em;
  color: white;
}

.value-display {
  position: absolute;
  top: 90%; /* 调整这个值来改变文本和圆弧之间的距离 */
  font-size: 1.2em;
  color: white;
}

.value-display input {
  font-size: 1.2em;
  text-align: center;
  width: 60px;
}
</style>
