<script setup lang="ts">
import { RouterView } from 'vue-router'
import AppBar from './components/AppBar/AppBar.vue';
import Message from '@/components/Message/Message.vue';
import { provide, ref } from 'vue';

var errorListener = 0;
var errorListener2 = 0;
var isError = ref(false);
var type = ref("success");
var errorText = ref("");

function showMsg(errorType: string, msg: string) {
	if (errorListener != 0) { clearTimeout(errorListener); }
	if (errorListener2 != 0) { clearTimeout(errorListener2); }
	isError.value = false;
	errorListener = setTimeout(() => { 
		isError.value = true;
		type.value = errorType;
		errorText.value = msg;
	}, 500);
	errorListener2 = setTimeout(() => { isError.value = false; }, 5000);
}

provide('showMsg', showMsg);
</script>

<template>
  <AppBar></AppBar>
  <div class="main-app">
    <div class="main-app-container">
      <router-view v-slot="{ Component }">
        <component :is="Component"></component>
      </router-view>
    </div>
  </div>
  <div class="nProcessBar">

  </div>
  <Message
    :isError="isError"
    :errorText="errorText"
    :type="type"
  ></Message>
</template>

<style scoped>
.nProcessBar {
  position: fixed;
  top: 53px;
  height: 10px;
  width: 100%;
  z-index: 10000;
}

.main-app {
  position: relative;
  top: 53px;
  min-height: calc(100vh - 107px);
  padding: 27px 0px;
  margin: auto;
}

.main-app-container {
  width: 100%;
  margin: auto;
}
@media (min-width: 1156px) {
  .main-app-container {
    width: 1156px;
  }
}
</style>
