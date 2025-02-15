<script lang="ts" setup>
import { i18n } from '@/i18n';
import { ref } from 'vue';

const props = defineProps([ 'current' ]);

const curr = ref([ props.current ]);
const t = i18n.global.t;
</script>

<template>
    <v-layout>
        <v-navigation-drawer
            expand-on-hover
            rail
            style="margin-top: 53px;"
            class="NavigationDrawer"
        >
            <v-list density="compact" nav v-model:selected="curr">
                <v-list-item prepend-icon="mdi-home" :title="t('pages.admin.index')" value="index"></v-list-item>
            </v-list>
        </v-navigation-drawer>    
    </v-layout>
</template>

<style scoped>
.NavigationDrawer {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
}
</style>