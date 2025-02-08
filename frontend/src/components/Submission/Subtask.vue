<script setup lang="ts">
import { statusList } from '@/config.ts'
import { i18n } from '@/i18n';
import SubmissionData from '@/components/Submission/Data.vue'

const t = i18n.global.t;
const props = defineProps([ 'subtask', 'judgeDatas' ]);
</script>

<template>
    <v-expansion-panel class="SubmissionSubtask card-radius">
        <v-expansion-panel-title class="d-flex align-center" style="padding: 0px 30px; min-height: 40px;">
            <p style="width: 18%">{{ subtask.title == "" ? t('pages.submissions.subtask', { id: subtask.id }) : subtask.title }}</p>
            <p class="SubmissionSubtask-status" :style="'color: ' + statusList[subtask.status].color">
                <v-icon :icon="statusList[subtask.status].icon" class="SubmissionSubtask-icon"></v-icon>
                &nbsp;
                {{ statusList[subtask.status].text }}
            </p>
            <p style="width: 18%">{{ t('pages.submissions.score') }}：&nbsp;{{ subtask.score }}</p>
        </v-expansion-panel-title>
        <v-expansion-panel-text style="padding-bottom: 0px;">
            <v-expansion-panels style="width: 100%" variant="accordion" static>
                <SubmissionData
                    v-for="(item, index) in subtask.datas"
                    :id="subtask.id + '-' + (index + 1)"
                    :data="item"
                    :reversed="1"
                    :judgeData="judgeDatas[index]"
                ></SubmissionData>
            </v-expansion-panels>
        </v-expansion-panel-text>
    </v-expansion-panel>
</template>

<style lang="css" scoped>
.SubmissionSubtask {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 5px;
}

.SubmissionSubtask-status {
    width: 27%; 
    position: relative;
    top: 2px;
}

.SubmissionSubtask-icon {
    position: relative;
    top: -1px;
}
</style>