<script setup lang="ts">
import { i18n } from '@/i18n';

const t = i18n.global.t;
const props = defineProps([ 'id', 'input', 'output' ]);
</script>

<template>
    <div class="d-flex mt-3" style="gap: 20px;">
        <div style="width: calc(50% - 10px)">
            <p>{{ t('pages.problems.casesInput', { id: id }) }}:</p>
            <pre class="d-flex mt-2">
                <code class="hljs">{{ input }}</code>
            </pre>
        </div>
        <div style="width: calc(50% - 10px)">
            <p>{{ t('pages.problems.casesOutput', { id: id }) }}:</p>
            <pre class="d-flex mt-2">
                <code class="hljs">{{ output }}</code>
            </pre>
        </div>
    </div>
</template>