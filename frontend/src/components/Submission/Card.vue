<script setup lang="ts">
import { statusList } from '@/config.ts'
const props = defineProps([ 'id', 'cid', 'pid', 'problem', 'uid', 'user', 'statusType', 'status', 'score' ]);
</script>

<template>
    <v-card class="d-flex SubmissionCard card-radius">
        <router-link :to="(cid == undefined ? '' : '/contests/' + cid) + '/submissions/' + id" class="SubmissionCard-id ellipsis">#{{ id }}</router-link>
        <router-link :to="'/users/' + uid" class="SubmissionCard-user ellipsis">{{ user }}</router-link>
        <router-link :to="(cid == undefined ? '' : '/contests/' + cid) + '/problems/' + pid" class="SubmissionCard-problem ellipsis">{{ problem }}</router-link>
        <router-link :to="(cid == undefined ? '' : '/contests/' + cid) + '/submissions/' + id" :style="'color:' + statusList[statusType].color">
            <v-icon :icon="statusList[statusType].icon" class="SubmissionCard-icon"></v-icon>
            &nbsp;
            {{ score }}
            &nbsp;
            {{ status }}
        </router-link>
    </v-card>
</template>

<style scoped>
.SubmissionCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding-left: 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.SubmissionCard-id {
    width: 10%;
}

.SubmissionCard-user {
    width: 15%;
}

.SubmissionCard-problem {
    width: 45%;
}

.SubmissionCard-icon {
    position: relative;
    top: -1px;
}
</style>