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
                <v-list-item prepend-icon="mdi-home" :title="t('pages.admin.index.title')" value="index" to="/admin/index"></v-list-item>
                <v-list-item prepend-icon="mdi-format-list-bulleted" :title="t('pages.admin.problems.title')" value="problems" to="/admin/problems/list"></v-list-item>
                <v-list-item prepend-icon="mdi-server" :title="t('pages.admin.submissions.title')" value="submissions" to="/admin/submissions/list"></v-list-item>
                <v-list-item prepend-icon="mdi-calendar-blank" :title="t('pages.admin.contests.title')" value="contests"></v-list-item>
                <v-list-item prepend-icon="mdi-tag" :title="t('pages.admin.tags.title')" value="tags"></v-list-item>
                <v-list-item prepend-icon="mdi-account" :title="t('pages.admin.users.title')" value="users"></v-list-item>
                <v-list-item prepend-icon="mdi-account-group" :title="t('pages.admin.groups.title')" value="groups"></v-list-item>
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