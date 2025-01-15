<script setup lang="ts">
import { i18n } from '@/i18n';

const t = i18n.global.t;
const props = defineProps([ 'info' ]);
</script>

<template>
    <v-expansion-panels :model-value="[0]">
        <v-expansion-panel class="SubmissionCompileInfo card-radius" static>
            <v-expansion-panel-title class="d-flex align-center" style="padding: 0px 20px 0px 10px;">
                <p class="v-card-title" style="padding-right: 0px">{{ t('pages.submissions.compileInfo') }}</p>
            </v-expansion-panel-title>
            <v-expansion-panel-text style="padding-bottom: 0px;">
                <pre class="d-flex">
                    <code class="hljs">{{ info }}</code>
                </pre>
            </v-expansion-panel-text>
        </v-expansion-panel>    
    </v-expansion-panels>
    
</template>

<style scoped>
.SubmissionCompileInfo {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 5px;
    width: 100%;
    margin-bottom: 20px;
}
</style>

<style>
.v-expansion-panel-text__wrapper {
    padding: 0 1rem;
}
</style>