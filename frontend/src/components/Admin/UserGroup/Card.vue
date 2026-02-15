<script setup lang="ts">
import { permissionList, statusList } from '@/config.ts'
import { i18n } from '@/i18n';
import { ref } from 'vue';
const title = defineModel("title");
const description = defineModel("description");
const users = defineModel("users");
const permission = defineModel("permission");
const props = defineProps([ 'id', 'count', 'usersList' ]);
const emits = defineEmits([ 'submit' ]);
const t = i18n.global.t;

const modelValue: any = ref([]);
const isPreview = ref(false);
</script>

<template>
    <v-expansion-panels static :model-value="modelValue">
        <v-expansion-panel class="UserGroupCard card-radius" style="padding: 10px;">
            <v-expansion-panel-title>
                <div>
                    <p style="font-size: 1.25rem;">{{ title }}（{{ t('pages.admin.groups.list.count', { count: count }) }}）</p>
                    <p class="mt-2">{{ description }}</p>
                </div>
            </v-expansion-panel-title>
            <v-expansion-panel-text>
                <div class="d-flex align-center">
                    <p>{{ t('pages.admin.groups.list.title') }}：</p>
                    <v-text-field
                        v-model="title"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mt-3">
                    <p>{{ t('pages.admin.groups.list.description') }}：</p>
                    <v-text-field
                        v-model="description"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mt-3">
                    <p>{{ t('pages.admin.groups.list.users') }}：</p>
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
                <p class="mt-3">{{ t('pages.admin.groups.list.permissionTitle') }}：</p>
                <div class="d-flex" style="flex-wrap: wrap;">
                    <p 
                        v-for="(perm, index) in permissionList"
                        style="width: 33.3%"
                        class="mt-1"
                    >
                        <v-icon
                            :icon="(permission as number) & (1 << index) ? 'mdi-checkbox-marked' : 'mdi-checkbox-blank-outline'"
                            @click="(permission as number) ^= (1 << index); modelValue = [0]"
                        ></v-icon>
                        <span>{{ t('pages.admin.groups.list.permission.' + perm) }}</span>
                    </p>
                </div>
                <div class="d-flex justify-center mt-3">
                    <v-btn 
                        @click="emits('submit', 1)" 
                        class="UserGroupCard-submitButton"
                    >{{ t('pages.admin.groups.list.submit') }}</v-btn>
                </div>
            </v-expansion-panel-text>
        </v-expansion-panel>
    </v-expansion-panels>
    <!-- <v-card>
        <v-card-title>{{ title }}</v-card-title>
        <v-card-subtitle>{{ description }}（{{ t('pages.admin.usergroups.count', { count: count }) }}）</v-card-subtitle>
        <v-card-text></v-card-text>
    </v-card> -->
</template>

<style scoped>
.UserGroupCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    margin-bottom: 20px;
    padding: 5px 15px;
    width: 100%;
    padding: 10px;
}

.UserGroupCard-button {
    color: var(--color-text)!important;     
    background-color: var(--color-background-mute)!important;
    position: relative;
    top: -1.5px;
    margin-left: 5px;
}

.UserGroupCard-submitButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>