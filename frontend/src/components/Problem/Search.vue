<script setup lang="ts">
import { difficultyList, tagsTypeList } from '@/config';
import { i18n } from '@/i18n';

const title: any = defineModel('title');
const tags: any = defineModel('tags');
const difficulties: any = defineModel('difficulties');

const t = i18n.global.t;
const props = defineProps([ 'tagsList' ]);
const emits = defineEmits([ 'search' ]);
const difficultiesList = difficultyList.map((item: any, index) => ({ id: index, title: item.title, color: item.color }));
</script>

<template>
    <v-expansion-panels static :model-value="[0]">
        <v-expansion-panel class="SubmissionSearch card-radius" style="padding: 10px;">
            <v-expansion-panel-title style="padding: 0px 20px; font-size: 1.25rem;">搜索</v-expansion-panel-title>
            <v-expansion-panel-text>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.search.title') }}:&nbsp;</p>
                    <v-text-field
                        v-model="title"
                        dense
                        hide-details
                        clearable
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.search.tags') }}:&nbsp;</p>
                    <v-autocomplete
                        v-model="tags"
                        :items="tagsList"
                        density="compact"
                        item-title="title"
                        item-value="id"
                        multiple
                        hide-details
                        clearable
                        chips
                    >
                        <template v-slot:chip="{ item, }">
                            <v-chip
                                size="small" 
                                :color="tagsTypeList[(item.raw as any).type].color" 
                                variant="flat"
                            >{{ (item.raw as any).title }}</v-chip>
                        </template>
                    </v-autocomplete>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.search.difficulties') }}:&nbsp;</p>
                    <v-autocomplete
                        v-model="difficulties"
                        :items="difficultiesList"
                        item-title="title"
                        item-value="id"
                        density="compact"
                        multiple
                        hide-details
                        clearable
                        chips
                    >
                        <template v-slot:chip="{ item, index }">
                            <v-chip
                                size="small" 
                                :color="item.raw.color" 
                                variant="flat"
                            >{{ item.raw.title }}</v-chip>
                        </template>
                    </v-autocomplete>
                </div>
                <div class="d-flex justify-center">
                    <v-btn 
                        @click="emits('search', 1)" 
                        class="SubmissionSearch-button"
                        size="small"
                    >{{ t('pages.problems.search.searchButton') }}</v-btn>
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
</style>