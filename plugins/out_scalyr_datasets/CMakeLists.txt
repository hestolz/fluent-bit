set(src
  scalyr_dataset_conf.c
  scalyr_dataset.c
  )

FLB_PLUGIN(out_scalyr_datasets "${src}" "")
