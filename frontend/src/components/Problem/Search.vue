<script setup lang="ts">
import { difficultyList, maxDifficulty, tagsTypeList } from '@/config';
import { i18n } from '@/i18n';

const title: any = defineModel('title');
const tags: any = defineModel('tags');
const minDiff: any = defineModel('minDiff');
const maxDiff: any = defineModel('maxDiff');

const t = i18n.global.t;
const props = defineProps([ 'tagsList' ]);
const emits = defineEmits([ 'search' ]);
const difficultiesList = difficultyList.map((item: any, index) => ({ id: index, title: item.title, color: item.color }));
</script>

<template>
    <v-expansion-panels static :model-value="[0]">
        <v-expansion-panel class="SubmissionSearch card-radius" style="padding: 10px;">
            <v-expansion-panel-title style="padding: 0px 20px; font-size: 1.25rem;">{{ t('pages.problems.searchTitle') }}</v-expansion-panel-title>
            <v-expansion-panel-text>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.search.title') }}：</p>
                    <v-text-field
                        v-model="title"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.search.tags') }}：</p>
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
                    <p>{{ t('pages.problems.search.minDifficulty') }}：</p>
                    <v-slider
                        v-model="minDiff"
                        :min="0"
                        :max="maxDifficulty"
                        :step="1"
                        class="align-center"
                        hide-details
                    >
                        <template v-slot:append>
                            <v-text-field
                                v-model="minDiff"
                                density="compact"
                                style="width: 100px"
                                type="number"
                                hide-details
                                single-line
                            ></v-text-field>
                        </template>
                    </v-slider>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.problems.search.maxDifficulty') }}：</p>
                    <v-slider
                        v-model="maxDiff"
                        :min="0"
                        :max="maxDifficulty"
                        :step="1"
                        class="align-center"
                        hide-details
                    >
                        <template v-slot:append>
                            <v-text-field
                                v-model="maxDiff"
                                density="compact"
                                style="width: 100px"
                                type="number"
                                hide-details
                                single-line
                            ></v-text-field>
                        </template>
                    </v-slider>
                </div>
                <div class="d-flex justify-center">
                    <v-btn 
                        @click="emits('search', 1)" 
                        class="SubmissionSearch-button"
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

p {
    flex-shrink: 0;
}
</style>