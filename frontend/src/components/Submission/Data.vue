<script setup lang="ts">
import { statusList } from '@/config.ts'
import { i18n } from '@/i18n';

const t = i18n.global.t;
const props = defineProps([ 'id', 'data', 'reversed', 'judgeData' ]);
</script>

<template>
    <v-expansion-panel 
        class="SubmissionData card-radius" 
        :style="'background-color: var(--color-background' + (reversed == 1 ? '-mute' : '') + ')!important;'"
    >
        <v-expansion-panel-title style="min-height: 40px; padding: 0px 30px;">
            <p style="width: 18%">{{ t('pages.submissions.case', { id: id }) }}</p>
            <p class="SubmissionData-status" :style="'color: ' + statusList[data.status].color">
                <v-icon :icon="statusList[data.status].icon" class="SubmissionData-icon"></v-icon>
                &nbsp;
                {{ statusList[data.status].text }}
            </p>
            <p style="width: 18%">{{ t('pages.submissions.score') }}：&nbsp;{{ data.score }}</p>
            <p style="width: 18%">{{ t('pages.submissions.time') }}：&nbsp;{{ data.time }}ms</p>
            <p style="width: 18%">{{ t('pages.submissions.memory') }}：&nbsp;{{ Math.round(data.memory / 1024) }}MB</p>
        </v-expansion-panel-title>
        <v-expansion-panel-text style="padding-bottom: 0px; width: 100%">
            <div>
                <div class="d-flex justify-space-between" style="padding: 0px 10px;">
                    <div class="d-flex align-center">
                        <p style="font-weight: 700">{{ t('pages.submissions.inputFile') }}</p>
                        <p>{{ judgeData.inputIgnored == 0 ? "" : "（" + t('pages.submissions.ignored', { size: judgeData.inputIgnored + " Bytes" }) + "）" }}</p>
                    </div>
                    <p>{{ judgeData.inputName }}</p>
                </div>
                <pre class="d-flex" style="margin-top: 10px;">
                    <code class="hljs" :style="'background-color: var(--color-background' + (reversed == 1 ? '' : '-mute') + ')!important;'">{{ judgeData.input }}</code>
                </pre>
            </div>
            <div>
                <div class="d-flex justify-space-between" style="padding: 0px 10px; margin-top: 10px;">
                    <div class="d-flex align-center">
                        <p style="font-weight: 700">{{ t('pages.submissions.answerFile') }}</p>
                        <p>{{ judgeData.outputIgnored == 0 ? "" : "（" + t('pages.submissions.ignored', { size: judgeData.outputIgnored + " Bytes" }) + "）" }}</p>
                    </div>
                    <p>{{ judgeData.outputName }}</p>
                </div>
                <pre class="d-flex" style="margin-top: 10px;">
                    <code class="hljs" :style="'background-color: var(--color-background' + (reversed == 1 ? '' : '-mute') + ')!important;'">{{ judgeData.output }}</code>
                </pre>
            </div>
            <div>
                <div class="d-flex justify-space-between" style="padding: 0px 10px; margin-top: 10px;">
                    <div class="d-flex align-center">
                        <p style="font-weight: 700">{{ t('pages.submissions.outputFile') }}</p>
                        <p>{{ data.outputIgnored == 0 ? "" : "（" + t('pages.submissions.ignored', { size: data.outputIgnored + " Bytes" }) + "）" }}</p>
                    </div>
                </div>
                <pre class="d-flex" style="margin-top: 10px;">
                    <code class="hljs" :style="'background-color: var(--color-background' + (reversed == 1 ? '' : '-mute') + ')!important;'">{{ data.output }}</code>
                </pre>    
            </div>
            <div v-if="data.info != ''">
                <div class="d-flex" style="padding: 0px 10px; margin-top: 10px;">
                    <p style="font-weight: 700">{{ t('pages.submissions.checkerOutput') }}</p>
                </div>
                <pre class="d-flex" style="margin-top: 10px;">
                    <code class="hljs" :style="'background-color: var(--color-background' + (reversed == 1 ? '' : '-mute') + ')!important;'">{{ data.info }}</code>
                </pre>    
            </div>
        </v-expansion-panel-text>
    </v-expansion-panel>    
</template>

<style scoped>
.SubmissionData {
    color: var(--color-text)!important;
    padding: 5px;
}

.SubmissionData-status {
    width: 27%; 
    position: relative;
    top: 2px;
}

.SubmissionData-icon {
    position: relative;
    top: -1px;
}

.hljs {
    width: 100%;
}
</style>