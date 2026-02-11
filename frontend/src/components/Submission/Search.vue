<script setup lang="ts">
import { statusList } from '@/config';
import { i18n } from '@/i18n';

const problems: any = defineModel('problems');
const users: any = defineModel('users');
const languages: any = defineModel('languages');
const status: any = defineModel('status');

const t = i18n.global.t;
const props = defineProps([ 'problemsList', 'usersList', 'languagesList', 'disableSearch' ]);
const emits = defineEmits([ 'search' ]);
const statusL = statusList.slice(0, 9).map((item: any, index) => ({ name: item.text, id: index }));
</script>

<template>
    <v-expansion-panels static :model-value="[0]">
        <v-expansion-panel class="SubmissionSearch card-radius" style="padding: 10px;">
            <v-expansion-panel-title style="padding: 0px 20px; font-size: 1.25rem;">{{ t('pages.submissions.searchTitle') }}</v-expansion-panel-title>
            <v-expansion-panel-text>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.submissions.search.problems') }}：</p>
                    <v-autocomplete
                        v-model="problems"
                        :items="problemsList"
                        item-title="name"
                        item-value="id"
                        density="compact"
                        multiple
                        hide-details
                        clearable
                    ></v-autocomplete>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.submissions.search.users') }}：</p>
                    <v-autocomplete
                        v-model="users"
                        :items="usersList"
                        item-title="title"
                        item-value="id"
                        density="compact"
                        multiple
                        hide-details
                        clearable
                    ></v-autocomplete>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.submissions.search.languages') }}：</p>
                    <v-autocomplete
                        v-model="languages"
                        :items="languagesList"
                        item-title="name"
                        item-value="id"
                        density="compact"
                        multiple
                        hide-details
                        clearable
                    ></v-autocomplete>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.submissions.search.status') }}：</p>
                    <v-autocomplete
                        v-model="status"
                        :items="statusL"
                        item-title="name"
                        item-value="id"
                        density="compact"
                        multiple
                        hide-details
                        clearable
                    ></v-autocomplete>
                </div>
                <div class="d-flex justify-center">
                    <v-btn 
                        @click="emits('search')" 
                        :disabled="disableSearch"
                        class="SubmissionSearch-button"
                    >{{ t('pages.submissions.search.searchButton') }}</v-btn>
                </div>
            </v-expansion-panel-text>
        </v-expansion-panel>
    </v-expansion-panels>
</template>

<style scoped>
.SubmissionSearch {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    margin-bottom: 20px;
    padding: 5px 15px;
    width: 100%;
}

.SubmissionSearch-button {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}

p {
    flex-shrink: 0;
}
</style>