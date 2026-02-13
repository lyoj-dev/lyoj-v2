<script setup lang="ts">
import { difficultyList, groupColor, maxDifficulty, statusCardList, tagsTypeList } from '@/config';
import { i18n } from '@/i18n';
import { goto, locate } from '@/router';
import { loginAs } from '@/utils';

const t = i18n.global.t;
const props = defineProps([ 'id', 'selected', 'rating', 'title', 'createTime', 'groups' ]);
const emits = defineEmits([ 'addGroup', 'updateSelected', 'addUserGroup', 'removeUserGroup' ]);

function formatTime(t: number) {
    var d = new Date(t * 1000);
    return d.getFullYear().toString() + '-' +
           (d.getMonth() + 1).toString().padStart(2, '0') + '-' + 
           d.getDate().toString().padStart(2, '0') + ' ' + 
           d.getHours().toString().padStart(2, '0') + ':' + 
           d.getMinutes().toString().padStart(2, '0') + ":" +
           d.getSeconds().toString().padStart(2, '0');
}
</script>

<template>
    <v-card class="d-flex UserCard card-radius">
        <div
            @click="$emit('updateSelected', !selected)"
            class="UserCard-status"
        >
            <v-icon
                :icon="selected ? 'mdi-checkbox-marked' : 'mdi-checkbox-blank-outline'"
            ></v-icon>
        </div>
        <router-link 
            :to="'/users/' + id" 
            class="UserCard-id ellipsis"
        >#{{ id }}</router-link>
        <router-link 
            :to="'/users/' + id" 
            class="UserCard-title ellipsis"
        >{{ title }}</router-link>
        <div class="UserCard-groups d-flex">
            <v-chip 
                v-for="group in groups" 
                :key="group" 
                size="small" 
                :color="groupColor" 
                variant="flat"
                @click="() => emits('addGroup', group.id)"
            >{{ group.title }}</v-chip>
        </div>
        <div class="UserCard-rating d-flex justify-center">
            <v-chip 
                :color="groupColor" 
                size="small" 
                variant="flat"
                v-if="rating != -1"
            >{{ t('pages.admin.users.list.rating') }}：{{ rating }}</v-chip>
        </div>
        <div class="UserCard-createTime d-flex justify-center">
            <v-chip 
                :color="groupColor" 
                size="small" 
                variant="flat"
                :title="t('pages.admin.users.list.createTime')"
                v-if="rating != -1"
            >{{ formatTime(createTime) }}</v-chip>
        </div>
        <div class="UserCard-actions d-flex justify-center align-center">
            <v-btn 
                class="UserCard-actionButton" 
                icon="mdi-plus" 
                size="x-small"
                :title="t('pages.admin.users.list.addUserGroupButton')"
                @click="emits('addUserGroup', id)"
            ></v-btn>
            <v-btn 
                class="UserCard-actionButton" 
                icon="mdi-minus" 
                size="x-small"
                :title="t('pages.admin.users.list.removeUserGroupButton')"
                @click="emits('removeUserGroup', id)"
            ></v-btn>
            <v-btn 
                class="UserCard-actionButton" 
                icon="mdi-pen" 
                size="x-small"
                :title="t('pages.admin.users.list.editButton')"
                @click="locate('/users/' + id + '?edit=1')"
            ></v-btn>
        </div>
    </v-card>
</template>

<style scoped>
.UserCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.UserCard-status {
    display: block;
    width: 3%;
    position: relative;
    top: -0.8px;
    padding: 3px;
    cursor: pointer;
}

.UserCard-id {
    width: 7%;
}

.UserCard-title {
    width: calc(35% - 37px);
}

.UserCard-groups {
    width: 20%;
    gap: 5px;
    flex-wrap: wrap;
    padding: 10px 0px;
}

.UserCard-rating {
    width: 10%;
}

.UserCard-createTime {
    width: 15%
}

.UserCard-actions {
    width: calc(10% + 37px);
    gap: 5px;
}

.UserCard-actionButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>