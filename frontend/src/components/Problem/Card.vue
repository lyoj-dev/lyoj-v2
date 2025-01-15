<script setup lang="ts">
import { difficultyList, tagsTypeList } from '@/config';

const props = defineProps([ 'status', 'id', 'alias', 'title', 'tags', 'accepted', 'total', 'difficulty' ]);
</script>

<template>
    <v-card class="d-flex ProblemCard card-radius">
        <router-link :to="'/submissions/list'" class="ProblemCard-status"><v-icon
            :icon="status == 0 ? 'mdi-check' : status == 1 ? 'mdi-close' : 'mdi-minus'"
        ></v-icon></router-link>
        <router-link :to="'/problems/' + id" class="ProblemCard-id">{{ alias }}</router-link>
        <router-link :to="'/problems/' + id" class="ProblemCard-problem">{{ title }}</router-link>
        <div class="ProblemCard-tags d-flex">
            <v-chip v-for="tag in tags" :key="tag" size="small" :color="tagsTypeList[tag.type].color" variant="flat">{{ tag.title }}</v-chip>
        </div>
        <div class="ProblemCard-difficulty d-flex justify-center">
            <v-chip :color="difficultyList[difficulty].color" size="small" variant="flat">{{ difficultyList[difficulty].title }}</v-chip>
        </div>
        <v-progress-linear 
            :model-value="accepted / total * 100" 
            :height="15" 
            color="green" 
            bg-color="currentColor" 
            class="ProblemCard-accepted"
        >{{ accepted }} / {{ total }}</v-progress-linear>
    </v-card>
</template>

<style scoped>
.ProblemCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding-left: 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.ProblemCard-status {
    width: 3%;
    position: relative;
    top: -0.8px;
}

.ProblemCard-id {
    width: 7%;
}

.ProblemCard-problem {
    width: 38%;
}

.ProblemCard-tags {
    width: 25%;
    gap: 5px;
    flex-wrap: wrap;
    padding: 10px 0px;
}

.ProblemCard-difficulty {
    width: 12%;
}

.ProblemCard-accepted {
    width: 10%;
    font-size: 12px;
}
</style>