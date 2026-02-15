<script setup lang="ts">
import { statusList } from '@/config.ts'
import { i18n } from '@/i18n';
const props = defineProps([ 'id', 'cid', 'pid', 'problem', 'uid', 'user', 'statusType', 'status', 'score', 'selected' ]);
const emits = defineEmits([ 'updateSelected', 'rejudge' ]);
const t = i18n.global.t;
</script>

<template>
    <v-card class="d-flex SubmissionCard card-radius">
        <div
            @click="$emit('updateSelected', !selected)"
            class="SubmissionCard-status"
        >
            <v-icon
                :icon="selected ? 'mdi-checkbox-marked' : 'mdi-checkbox-blank-outline'"
            ></v-icon>
        </div>
        <router-link :to="(cid == undefined || cid == 0 ? '' : '/contests/' + cid) + '/submissions/' + id" class="SubmissionCard-id ellipsis">#{{ id }}</router-link>
        <router-link :to="'/users/' + uid" class="SubmissionCard-user ellipsis">{{ user }}</router-link>
        <router-link :to="(cid == undefined || cid == 0 ? '' : '/contests/' + cid) + '/problems/' + pid" class="SubmissionCard-problem ellipsis">{{ problem }}</router-link>
        <router-link :to="(cid == undefined || cid == 0 ? '' : '/contests/' + cid) + '/submissions/' + id" :style="'color:' + statusList[statusType].color" class="SubmissionCard-results">
            <v-icon :icon="statusList[statusType].icon" class="SubmissionCard-icon"></v-icon>
            &nbsp;
            {{ score }}
            &nbsp;
            {{ status }}
        </router-link>
        <div class="SubmissionCard-actions d-flex justify-center align-center">
            <v-btn 
                class="SubmissionCard-actionButton" 
                icon="mdi-restart" 
                size="x-small"
                :title="t('pages.admin.submissions.list.rejudgeButton')"
                @click="() => emits('rejudge', id)"
            ></v-btn>
        </div>
    </v-card>
</template>

<style scoped>
.SubmissionCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.SubmissionCard-status {
    display: block;
    width: 3%;
    position: relative;
    top: -0.8px;
    padding: 3px;
    cursor: pointer;
}

.SubmissionCard-id {
    width: 7%;
}

.SubmissionCard-user {
    width: 15%;
}

.SubmissionCard-problem {
    width: calc(40% + 37px);
}

.SubmissionCard-results {
    width: 25%;
}

.SubmissionCard-icon {
    position: relative;
    top: -1px;
}

.SubmissionCard-actions {
    width: calc(10% - 37px);
    gap: 5px;
}

.SubmissionCard-actionButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>