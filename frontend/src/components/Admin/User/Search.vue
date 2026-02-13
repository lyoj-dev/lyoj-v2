<script setup lang="ts">
import { groupColor, maxRating } from '@/config';
import { i18n } from '@/i18n';

const title: any = defineModel('title');
const groups: any = defineModel('groups');
const minRatingValue: any = defineModel('minRating');
const maxRatingValue: any = defineModel('maxRating');

const t = i18n.global.t;
const props = defineProps([ 'groupsList', 'disableSearch' ]);
const emits = defineEmits([ 'search' ]);
</script>

<template>
    <v-expansion-panels static :model-value="[0]">
        <v-expansion-panel class="UserSearch card-radius" style="padding: 10px;">
            <v-expansion-panel-title style="padding: 0px 20px; font-size: 1.25rem;">{{ t('pages.admin.users.list.searchTitle') }}</v-expansion-panel-title>
            <v-expansion-panel-text>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.list.title') }}：</p>
                    <v-text-field
                        v-model="title"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.list.groups') }}：</p>
                    <v-autocomplete
                        v-model="groups"
                        :items="groupsList"
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
                                :color="groupColor" 
                                variant="flat"
                            >{{ (item.raw as any).title }}</v-chip>
                        </template>
                    </v-autocomplete>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.list.minRating') }}：</p>
                    <v-slider
                        v-model="minRatingValue"
                        :min="0"
                        :max="maxRating"
                        :step="1"
                        class="align-center"
                        hide-details
                    >
                        <template v-slot:append>
                            <v-text-field
                                v-model="minRatingValue"
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
                    <p>{{ t('pages.admin.users.list.maxRating') }}：</p>
                    <v-slider
                        v-model="maxRatingValue"
                        :min="0"
                        :max="maxRating"
                        :step="1"
                        class="align-center"
                        hide-details
                    >
                        <template v-slot:append>
                            <v-text-field
                                v-model="maxRatingValue"
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
                        :disabled="disableSearch"
                        class="UserSearch-button"
                    >{{ t('pages.admin.users.list.searchButton') }}</v-btn>
                </div>
            </v-expansion-panel-text>
        </v-expansion-panel>
    </v-expansion-panels>
</template>

<style scoped>
.UserSearch {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    margin-bottom: 20px;
    padding: 5px 15px;
    width: 100%;
}

.UserSearch-button {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}

p {
    flex-shrink: 0;
}
</style>