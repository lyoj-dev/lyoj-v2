<script setup lang="ts">
const props = defineProps([ 'alias', 'title', 'inputFile', 'outputFile', 'minTime', 'maxTime', 'minMemory', 'maxMemory' ]);
</script>

<template>
    <v-card class="ProblemTitle card-radius">
        <v-card-title class="ProblemTitle-title">
            {{ alias }} - {{ title }}
        </v-card-title>
        <v-card-text>
            输入文件: {{ inputFile }} | 
            输出文件: {{ outputFile }} | 
            时间限制: {{ minTime == maxTime ? minTime + 'ms' : minTime + 'ms ~ ' + maxTime + 'ms' }} | 
            内存限制: {{ 
                Math.round(minMemory / 1024) == Math.round(maxMemory / 1024) 
                ? Math.round(minMemory / 1024) + 'MB'
                : Math.round(minMemory / 1024) + 'MB ~ ' + Math.round(maxMemory / 1024) + 'MB'
            }}
        </v-card-text>
    </v-card>
</template>

<style scoped>
.ProblemTitle {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px;
}

.ProblemTitle-title {
    font-size: 25px;
    font-weight: 400;
}
</style>