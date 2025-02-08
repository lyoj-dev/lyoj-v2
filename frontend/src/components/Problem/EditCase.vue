<script setup lang="ts">
import { i18n } from '@/i18n';

const t = i18n.global.t;
const props = defineProps([ 'id' ]);
const input = defineModel('input');
const output = defineModel('output');
</script>

<template>
    <div class="d-flex mt-3" style="gap: 20px;">
        <div style="width: calc(50% - 10px)">
            <p>{{ t('pages.problems.edit.casesInput', { id: id }) }}：</p>
            <v-textarea
                v-model="input"
                hide-details
                auto-grow
                density="compact"
                class="mt-2"
                style="font-family: 'Cascadia Mono', monospace!important;"
            ></v-textarea>
        </div>
        <div style="width: calc(50% - 10px)">
            <p>{{ t('pages.problems.edit.casesOutput', { id: id }) }}：</p>
            <v-textarea
                v-model="output"
                hide-details
                auto-grow
                density="compact"
                class="mt-2"
                style="font-family: 'Cascadia Mono', monospace!important;"
            ></v-textarea>
        </div>
    </div>
</template>