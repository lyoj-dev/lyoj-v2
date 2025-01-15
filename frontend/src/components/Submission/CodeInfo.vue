<script setup lang="ts">
const props = defineProps([ 'icon', 'info' ]);
</script>

<template>
    <v-icon :icon="icon" size="x-small" class="SubmissionCodeInfo-icon"></v-icon>
    <p class="SubmissionCodeInfo-info">{{ info }}</p>
</template>

<style scoped>
.SubmissionCodeInfo-icon {
    position: relative;
    top: 2px;
    margin-left: 5px;
}

.SubmissionCodeInfo-info {
    font-size: 15px;
    margin-left: 3px;
    margin-right: 5px;
    position: relative;
    top: 1.5px;
}
</style>