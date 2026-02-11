<script lang="ts" setup>
import { contestStatusCardList, tagsTypeList } from '@/config';
import { i18n } from '@/i18n';
import { locate } from '@/router';
import { loginAs } from '@/utils';
import { computed } from 'vue';

const t = i18n.global.t;
const props = defineProps([ 'id', 'title', 'tags', 'startTime', 'endTime', 'problems' ]);
const emits = defineEmits([ 'edit' ])

const status = computed(() => {
    var now = Math.floor(Date.now() / 1000);
    if (now < props.startTime) return 0;
    if (props.endTime < now) return 2;
    return 1;
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
    <v-card class="d-flex ContestCard card-radius">
        <div
            class="ContestCard-status"
            :style="'color: ' + contestStatusCardList[status].color + '!important;'"
        >{{ contestStatusCardList[status].title }}</div>
        <router-link :to="'/contests/' + id + '/index'" class="ContestCard-title ellipsis">{{ title }}</router-link>
        <div class="ContestCard-tags d-flex">
            <v-chip 
                v-for="tag in tags" 
                :key="tag" 
                size="small" 
                :color="tagsTypeList[tag.type].color" 
                variant="flat"
            >{{ tag.title }}</v-chip>
        </div>
        <p class="ContestCard-time">{{ formatTime(startTime) + ' ～ ' + formatTime(endTime) }}</p>
        <p class="ContestCard-problems">{{ t('pages.contests.problemNumber') }}：{{ problems }}</p>
        <div class="ContestCard-actions d-flex justify-center align-center">
            <v-btn 
                class="ContestCard-actionButton" 
                icon="mdi-pen"
                size="x-small"
                :title="t('pages.admin.contests.list.editButton')"
                @click="() => emits('edit', id)"
            ></v-btn>
        </div>
    </v-card>
</template>

<style scoped>
.ContestCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.ContestCard-status {
    width: 10%;
}

.ContestCard-title {
    width: calc(30% + 37px);
}

.ContestCard-tags {
    width: 20%;
    gap: 5px;
    flex-wrap: wrap;
    padding: 10px 0px;
}

.ContestCard-time {
    width: 20%;
}

.ContestCard-problems {
    width: 10%;
}

.ContestCard-actions {
    width: calc(10% - 37px);
    gap: 5px;
}

.ContestCard-actionButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>