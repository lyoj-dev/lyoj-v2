<script setup lang="ts">
import { computed } from "vue";

const props = defineProps(['isError', 'errorText', 'type']);

var isError = computed(() => props.isError);
var errorText = computed(() => props.errorText);
var type = computed(() => props.type);
</script>

<template>
    <div :style="(isError ? 'opacity: 1;' : 'opacity: 0;') + 'transition: opacity 0.5s; z-index: 9999; top: 80px; right: 16px; position: fixed'">
        <v-alert
            :type="type"
            style="min-width: 300px;"
        >
            <template v-slot:text>
                <div v-html="errorText"></div>
            </template>
        </v-alert>
    </div>
</template>