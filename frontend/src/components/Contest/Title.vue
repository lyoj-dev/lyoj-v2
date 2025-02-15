<script setup lang="ts">
import { i18n } from '@/i18n';
import { onUnmounted, ref } from 'vue';

const props = defineProps([ 'id', 'title', 'startTime', 'endTime', 'signups', 'problems' ]);
const t = i18n.global.t;

const currentTime = ref(Math.floor(Date.now() / 1000));
var timer = setInterval(() => {
    currentTime.value = Math.floor(Date.now() / 1000);
}, 1000);
onUnmounted(() => {
    clearInterval(timer);
});

function formatTime(t: number) {
    var d = new Date(t * 1000);
    return (d.getMonth() + 1).toString().padStart(2, '0') + '-' + 
           d.getDate().toString().padStart(2, '0') + ' ' + 
           d.getHours().toString().padStart(2, '0') + ':' + 
           d.getMinutes().toString().padStart(2, '0');
}
</script>

<template>
    <v-card class="ContestTitle card-radius">
        <v-card-title class="ContestTitle-title">
            #{{ id }} - {{ title }}
        </v-card-title>
        <v-card-text>
            {{ t('pages.contests.time') }}：{{ formatTime(startTime) }}～{{ formatTime(endTime) }} | 
            {{ t('pages.contests.signups') }}：{{ signups }} | 
            {{ t('pages.contests.problemNumber', { number: problems }) }}：{{ problems }}
        </v-card-text>
    </v-card>
    <v-card class="ContestTitle-timebar card-radius">
        <div class="d-flex justify-center align-center">
            <p style="flex-shrink: 0;">{{ formatTime(startTime) }}</p>
            <v-progress-linear
                color="orange"
                height="10"
                :model-value="Math.min(100, Math.max(0, (currentTime - startTime) / (endTime - startTime) * 100))"
                striped
                style="margin: 0px 10px;"
            ></v-progress-linear>
            <p style="flex-shrink: 0;">{{ formatTime(endTime) }}</p>
        </div>
    </v-card>
</template>

<style scoped>
.ContestTitle {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px;
}

.ContestTitle-title {
    font-size: 25px;
    font-weight: 400;
}

.ContestTitle-timebar {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 10px 15px;
}
</style>