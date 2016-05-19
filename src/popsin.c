/*
 *  popsin.c
 *  This file is part of LIME, the versatile line modeling engine
 *
 *  Copyright (C) 2006-2014 Christian Brinch
 *  Copyright (C) 2015 The LIME development team
 *
 */

#include "lime.h"


void
popsin(inputPars *par, struct grid **gp, molData **md, int *popsdone){
  FILE *fp;
  int i,j,k;
  double dummy;

  if((fp=fopen(par->restart, "rb"))==NULL){
    if(!silent) bail_out("Error reading binary output populations file!");
    exit(1);
  }

  par->numDensities=1;
  fread(&par->radius,   sizeof(double), 1, fp);
  fread(&par->ncell,    sizeof(int), 1, fp);
  fread(&par->nSpecies, sizeof(int), 1, fp);
  if( par->nSpecies < 0 || par->nSpecies > MAX_NSPECIES )
    {
      if(!silent) bail_out("Error reading binary output populations file : is this really a binary output file generated by lime ?");
      exit(1);
    }

  *md=realloc(*md, sizeof(molData)*par->nSpecies);

  for(i=0;i<par->nSpecies;i++){
    (*md)[i].lcl = NULL;
    (*md)[i].lcu = NULL;
    (*md)[i].eterm = NULL;
    (*md)[i].gstat = NULL;
    (*md)[i].cmb = NULL;
    fread(&(*md)[i].nlev,  sizeof(int),        1,fp);
    fread(&(*md)[i].nline, sizeof(int),        1,fp);
    fread(&(*md)[i].npart, sizeof(int),        1,fp);
    (*md)[i].ntrans=malloc(sizeof(int)*(*md)[i].npart);
    for(j=0;j<(*md)[i].npart;j++) fread(&(*md)[i].ntrans[j], sizeof(int), 1,fp);
    (*md)[i].lal=malloc(sizeof(int)*(*md)[i].nline);
    for(j=0;j<(*md)[i].nline;j++) fread(&(*md)[i].lal[j],    sizeof(int), 1,fp);
    (*md)[i].lau=malloc(sizeof(int)*(*md)[i].nline);
    for(j=0;j<(*md)[i].nline;j++) fread(&(*md)[i].lau[j],    sizeof(int), 1,fp);
    (*md)[i].aeinst=malloc(sizeof(double)*(*md)[i].nline);
    for(j=0;j<(*md)[i].nline;j++) fread(&(*md)[i].aeinst[j], sizeof(double), 1,fp);
    (*md)[i].freq=malloc(sizeof(double)*(*md)[i].nline);
    for(j=0;j<(*md)[i].nline;j++) fread(&(*md)[i].freq[j],   sizeof(double), 1,fp);
    (*md)[i].beinstl=malloc(sizeof(double)*(*md)[i].nline);
    for(j=0;j<(*md)[i].nline;j++) fread(&(*md)[i].beinstl[j],sizeof(double), 1,fp);
    (*md)[i].beinstu=malloc(sizeof(double)*(*md)[i].nline);
    for(j=0;j<(*md)[i].nline;j++) fread(&(*md)[i].beinstu[j],sizeof(double), 1,fp);
    (*md)[i].local_cmb = malloc(sizeof(double)*(*md)[i].nline);
    for(j=0;j<(*md)[i].nline;j++) fread(&(*md)[i].local_cmb[j],sizeof(double), 1,fp);
    fread(&(*md)[i].norm, sizeof(double),      1,fp);
    fread(&(*md)[i].norminv, sizeof(double),   1,fp);
  }

  *gp=malloc(sizeof(struct grid)*par->ncell);

  for(i=0;i<par->ncell;i++){
    (*gp)[i].a0 = NULL;
    (*gp)[i].a1 = NULL;
    (*gp)[i].a2 = NULL;
    (*gp)[i].a3 = NULL;
    (*gp)[i].a4 = NULL;
    (*gp)[i].dens = NULL;
    (*gp)[i].abun = NULL;
    (*gp)[i].nmol = NULL;
    (*gp)[i].dir = NULL;
    (*gp)[i].neigh = NULL;
    (*gp)[i].w = NULL;
    (*gp)[i].ds = NULL;
    fread(&(*gp)[i].id, sizeof (*gp)[i].id, 1, fp);
    fread(&(*gp)[i].x, sizeof (*gp)[i].x, 1, fp);
    fread(&(*gp)[i].vel, sizeof (*gp)[i].vel, 1, fp);
    fread(&(*gp)[i].sink, sizeof (*gp)[i].sink, 1, fp);
    (*gp)[i].nmol=malloc(par->nSpecies*sizeof(double));
    for(j=0;j<par->nSpecies;j++) fread(&(*gp)[i].nmol[j], sizeof(double), 1, fp);
    fread(&(*gp)[i].dopb, sizeof (*gp)[i].dopb, 1, fp);
    (*gp)[i].mol=malloc(par->nSpecies*sizeof(struct populations));
    for(j=0;j<par->nSpecies;j++){
      (*gp)[i].mol[j].pops=malloc(sizeof(double)*(*md)[j].nlev);
      for(k=0;k<(*md)[j].nlev;k++) fread(&(*gp)[i].mol[j].pops[k], sizeof(double), 1, fp);
      (*gp)[i].mol[j].knu=malloc(sizeof(double)*(*md)[j].nline);
      for(k=0;k<(*md)[j].nline;k++) fread(&(*gp)[i].mol[j].knu[k], sizeof(double), 1, fp);
      (*gp)[i].mol[j].dust=malloc(sizeof(double)*(*md)[j].nline);
      for(k=0;k<(*md)[j].nline;k++) fread(&(*gp)[i].mol[j].dust[k],sizeof(double), 1, fp);
      fread(&(*gp)[i].mol[j].dopb,sizeof(double), 1, fp);
      fread(&(*gp)[i].mol[j].binv,sizeof(double), 1, fp);
      (*gp)[i].mol[j].partner=NULL;
    }
    fread(&dummy, sizeof(double), 1, fp);
    fread(&dummy, sizeof(double), 1, fp);
    fread(&dummy, sizeof(double), 1, fp);
  }
  fclose(fp);

  qhull(par, *gp);
  distCalc(par, *gp);
  getVelosplines(par,*gp);
  *popsdone=1;
}

