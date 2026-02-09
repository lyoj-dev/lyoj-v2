<script setup lang="ts">
import { RouterView } from 'vue-router'
import AppBar from './components/AppBar/AppBar.vue';
import Message from '@/components/Message/Message.vue';
import { provide } from 'vue';
import { showMsg, isError, type, errorText } from '@/utils';
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
  z-index: 1005;
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
