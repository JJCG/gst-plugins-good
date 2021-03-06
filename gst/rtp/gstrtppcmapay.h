/* GStreamer
 * Copyright (C) <2005> Edgard Lima <edgard.lima@indt.org.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more 
 */


#ifndef __GST_RTP_PCMA_PAY_H__
#define __GST_RTP_PCMA_PAY_H__

#include <gst/gst.h>
#include <gst/rtp/gstbasertpaudiopayload.h>

G_BEGIN_DECLS

typedef struct _GstRtpPmcaPay GstRtpPmcaPay;
typedef struct _GstRtpPmcaPayClass GstRtpPmcaPayClass;

#define GST_TYPE_RTP_PCMA_PAY \
  (gst_rtp_pcma_pay_get_type())
#define GST_RTP_PCMA_PAY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_RTP_PCMA_PAY,GstRtpPmcaPay))
#define GST_RTP_PCMA_PAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_RTP_PCMA_PAY,GstRtpPmcaPayClass))
#define GST_IS_RTP_PCMA_PAY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_RTP_PCMA_PAY))
#define GST_IS_RTP_PCMA_PAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_RTP_PCMA_PAY))

struct _GstRtpPmcaPay
{
  GstBaseRTPAudioPayload audiopayload;
};

struct _GstRtpPmcaPayClass
{
  GstBaseRTPAudioPayloadClass parent_class;
};

gboolean gst_rtp_pcma_pay_plugin_init (GstPlugin * plugin);

G_END_DECLS

#endif /* __GST_RTP_PCMA_PAY_H__ */
