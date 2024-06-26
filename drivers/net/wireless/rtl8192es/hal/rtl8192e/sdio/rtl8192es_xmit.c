/******************************************************************************
 *
 * Copyright(c) 2012 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTL8192ES_XMIT_C_

#include <drv_types.h>
#include <rtl8192e_hal.h>


/* #define CONFIG_FIX_CORE_DUMP ==> have bug */
/* #define DBG_EMINFO */

void rtl8192es_fill_default_txdesc(
	struct xmit_frame *pxmitframe,
	u8 *ptxdesc)

{
	PADAPTER padapter;
	PHAL_DATA_TYPE pHalData;
	struct mlme_ext_priv *pmlmeext;
	struct mlme_ext_info *pmlmeinfo;
	struct pkt_attrib *pattrib;
	s32 bmcst;
	u8 offset = TXDESC_SIZE;

	padapter = pxmitframe->padapter;
	pHalData = GET_HAL_DATA(padapter);
	pmlmeext = &padapter->mlmeextpriv;
	pmlmeinfo = &(pmlmeext->mlmext_info);

	pattrib = &pxmitframe->attrib;
	bmcst = IS_MCAST(pattrib->ra);

	if (bmcst)
		SET_TX_DESC_BMC_92E(ptxdesc, 1);

	SET_TX_DESC_PKT_SIZE_92E(ptxdesc, pattrib->last_txcmdsz);
#ifdef CONFIG_TX_EARLY_MODE
	if (pxmitframe->frame_tag == DATA_FRAMETAG) {
		offset += EARLY_MODE_INFO_SIZE;
		pxmitframe->pkt_offset = 0x01;
	}
#endif /* CONFIG_TX_EARLY_MODE */

	SET_TX_DESC_OFFSET_92E(ptxdesc, offset);

	/* SET_TX_DESC_PKT_OFFSET_92E(ptxdesc, pxmitframe->pkt_offset); */

	SET_TX_DESC_MACID_92E(ptxdesc, pattrib->mac_id);
	SET_TX_DESC_RATE_ID_92E(ptxdesc, pattrib->raid);
	SET_TX_DESC_QUEUE_SEL_92E(ptxdesc,  pattrib->qsel);

	/* 2009.11.05. tynli_test. Suggested by SD4 Filen for FW LPS. */
	/* (1) The sequence number of each non-Qos frame / broadcast / multicast / */
	/* mgnt frame should be controled by Hw because Fw will also send null data */
	/* which we cannot control when Fw LPS enable. */
	/* --> default enable non-Qos data sequense number. 2010.06.23. by tynli. */
	/* (2) Enable HW SEQ control for beacon packet, because we use Hw beacon. */
	/* (3) Use HW Qos SEQ to control the seq num of Ext port non-Qos packets. */
	/* 2010.06.23. Added by tynli. */
	if (!pattrib->qos_en) {
		SET_TX_DESC_EN_HWSEQ_92E(ptxdesc, 1); /* Hw set sequence number */
		SET_TX_DESC_HWSEQ_SEL_92E(ptxdesc, pattrib->hw_ssn_sel);
	} else
		SET_TX_DESC_SEQ_92E(ptxdesc, pattrib->seqnum);


	if (pxmitframe->frame_tag == DATA_FRAMETAG) {
		fill_txdesc_sectype(pattrib, ptxdesc);
#if defined(CONFIG_CONCURRENT_MODE)
		if (bmcst)
			fill_txdesc_force_bmc_camid(pattrib, ptxdesc);
#endif

		if ((pattrib->ether_type != 0x888e) &&
		    (pattrib->ether_type != 0x0806) &&
		    (pattrib->ether_type != 0x88b4) &&
		    (pattrib->dhcp_pkt != 1)
#ifdef CONFIG_AUTO_AP_MODE
		    && (pattrib->pctrl != _TRUE)
#endif
		   ) {
			/* Non EAP & ARP & DHCP type data packet */
			fill_txdesc_vcs(pattrib, ptxdesc);
			fill_txdesc_phy(padapter, pattrib, ptxdesc);

			if (pattrib->ampdu_en == _TRUE) {
				SET_TX_DESC_AGG_ENABLE_92E(ptxdesc, 1);
				SET_TX_DESC_MAX_AGG_NUM_92E(ptxdesc, 0x1f);
				/* Set A-MPDU aggregation. */
				SET_TX_DESC_AMPDU_DENSITY_92E(ptxdesc, pattrib->ampdu_spacing);
			} else
				SET_TX_DESC_BK_92E(ptxdesc, 1);

			SET_TX_DESC_RTS_RATE_92E(ptxdesc, 0x8);/* RTS Rate=24M */
			/* DATA/RTS  Rate FB LMT */
			SET_TX_DESC_DATA_RATE_FB_LIMIT_92E(ptxdesc, 0x1f);
			SET_TX_DESC_RTS_RATE_FB_LIMIT_92E(ptxdesc, 0xf);


			if (pHalData->fw_ractrl == _FALSE) {
				SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);

				if (pHalData->INIDATA_RATE[pattrib->mac_id] & BIT(7))
					SET_TX_DESC_DATA_SHORT_92E(ptxdesc, 1);

				SET_TX_DESC_TX_RATE_92E(ptxdesc, (pHalData->INIDATA_RATE[pattrib->mac_id] & 0x7F));
			}

			if (bmcst)
				fill_txdesc_bmc_tx_rate(pattrib, ptxdesc);

			/* for debug */
			rtl8192e_fixed_rate(padapter, ptxdesc);

			if (pattrib->ldpc)
				SET_TX_DESC_DATA_LDPC_92E(ptxdesc, 1);
			if (pattrib->stbc)
				SET_TX_DESC_DATA_STBC_92E(ptxdesc, 1);
		} else {
			/* EAP data packet and ARP and DHCP packet. */
			/* Use the 1M or 6M data rate to send the EAP/ARP packet. */
			/* This will maybe make the handshake smooth. */

			SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);

			SET_TX_DESC_BK_92E(ptxdesc, 1);

			if (pmlmeinfo->preamble_mode == PREAMBLE_SHORT)
				SET_TX_DESC_DATA_SHORT_92E(ptxdesc, 1);

			SET_TX_DESC_TX_RATE_92E(ptxdesc, MRateToHwRate(pmlmeext->tx_rate));
		}

		SET_TX_DESC_USB_TXAGG_NUM_92E(ptxdesc, pxmitframe->agg_num);

#ifdef CONFIG_TDLS
#ifdef CONFIG_XMIT_ACK
		/* CCX-TXRPT ack for xmit mgmt frames. */
		if (pxmitframe->ack_report) {
#ifdef DBG_CCX
			RTW_INFO("%s set spe_rpt\n", __func__);
#endif
			SET_TX_DESC_SPE_RPT_92E(ptxdesc, 1);
		}
#endif /* CONFIG_XMIT_ACK */
#endif
	} else if (pxmitframe->frame_tag == MGNT_FRAMETAG) {
		/*		RTW_INFO("%s: MGNT_FRAMETAG\n", __FUNCTION__); */

		SET_TX_DESC_MBSSID_92E(ptxdesc, pattrib->mbssid);/* for issue port1/mbssid beacon */

		SET_TX_DESC_RETRY_LIMIT_ENABLE_92E(ptxdesc, 1);/* retry limit enable */
		if (pattrib->retry_ctrl == _TRUE)
			SET_TX_DESC_DATA_RETRY_LIMIT_92E(ptxdesc, 6);
		else
			SET_TX_DESC_DATA_RETRY_LIMIT_92E(ptxdesc, 12);

		SET_TX_DESC_USE_RATE_92E(ptxdesc, 1); /* driver uses rate, 1M */

		SET_TX_DESC_TX_RATE_92E(ptxdesc, MRateToHwRate(pattrib->rate));

#ifdef CONFIG_XMIT_ACK
		/* CCX-TXRPT ack for xmit mgmt frames. */
		if (pxmitframe->ack_report) {
#ifdef DBG_CCX
			RTW_INFO("%s set spe_rpt\n", __FUNCTION__);
#endif
			SET_TX_DESC_SPE_RPT_92E(ptxdesc, 1);
		}
#endif /* CONFIG_XMIT_ACK */
	}
	/*
		else if (pxmitframe->frame_tag == TXAGG_FRAMETAG)
		{
			RTW_INFO("%s: TXAGG_FRAMETAG\n", __FUNCTION__);
		}
	*/
#ifdef CONFIG_MP_INCLUDED
	else if ((pxmitframe->frame_tag == MP_FRAMETAG)
		 && (padapter->registrypriv.mp_mode == 1)) {
		/* RTW_INFO("%s: MP_FRAMETAG\n", __FUNCTION__); */
		fill_txdesc_for_mp(padapter, (u8 *)ptxdesc);
	}
#endif /* CONFIG_MP_INCLUDED */
	else {
		RTW_INFO("%s: frame_tag=0x%x\n", __FUNCTION__, pxmitframe->frame_tag);

		SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);
		SET_TX_DESC_TX_RATE_92E(ptxdesc, MRateToHwRate(pmlmeext->tx_rate));
	}

#ifdef CONFIG_ANTENNA_DIVERSITY
	if (!bmcst && pattrib->psta)
		odm_set_tx_ant_by_tx_info(adapter_to_phydm(padapter), ptxdesc, pattrib->psta->cmn.mac_id);
#endif

	rtl8192e_cal_txdesc_chksum(ptxdesc);
	_dbg_dump_tx_info(padapter, pxmitframe->frame_tag, ptxdesc);

}
#if 0
{
	PADAPTER padapter = pxmitframe->padapter;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	s32 bmcst = IS_MCAST(pattrib->ra);
	u8 *ptxdesc = pbuf;


	/* RTW_INFO("%s==> pkt_len=%d,bagg_pkt=%02x\n",__FUNCTION__,sz,bagg_pkt); */
	SET_TX_DESC_PKT_SIZE_92E(ptxdesc, sz);

	offset = TXDESC_SIZE + OFFSET_SZ;

#ifdef CONFIG_TX_EARLY_MODE
	if (bagg_pkt) {
		offset += EARLY_MODE_INFO_SIZE ;/* 0x28			 */
	}
#endif
	/* RTW_INFO("%s==>offset(0x%02x)\n",__FUNCTION__,offset); */
	SET_TX_DESC_OFFSET_92E(ptxdesc, offset);

	if (bmcst)
		SET_TX_DESC_BMC_92E(ptxdesc, 1);

	/* RTW_INFO("%s, pkt_offset=0x%02x\n",__FUNCTION__,pxmitframe->pkt_offset); */
	/* pkt_offset, unit:8 bytes padding */
	if (pxmitframe->pkt_offset > 0)
		SET_TX_DESC_PKT_OFFSET_92E(ptxdesc, pxmitframe->pkt_offset);

	SET_TX_DESC_MACID_92E(ptxdesc, pattrib->mac_id);
	SET_TX_DESC_RATE_ID_92E(ptxdesc, pattrib->raid);

	SET_TX_DESC_QUEUE_SEL_92E(ptxdesc,  pattrib->qsel);

	/* offset 12 */

	if (!pattrib->qos_en) {
		SET_TX_DESC_EN_HWSEQ_92E(ptxdesc, 1); /* Hw set sequence number */
	} else
		SET_TX_DESC_SEQ_92E(ptxdesc, pattrib->seqnum);



	if (pxmitframe->frame_tag == DATA_FRAMETAG) {
		ptxdesc->macid = pattrib->mac_id; /* CAM_ID(MAC_ID) */

		if (pattrib->ampdu_en == _TRUE) {
			ptxdesc->agg_en = 1; /* AGG EN */
			ptxdesc->ampdu_density = pattrib->ampdu_spacing;
		} else {
			ptxdesc->bk = 1; /* AGG BK */
		}

		ptxdesc->qsel = pattrib->qsel;
		ptxdesc->rate_id = pattrib->raid;

		fill_txdesc_sectype(pattrib, ptxdesc);

		ptxdesc->seq = pattrib->seqnum;

		/* todo: qos_en */

		ptxdesc->userate = 1; /* driver uses rate	 */

		if ((pattrib->ether_type != 0x888e) &&
		    (pattrib->ether_type != 0x0806) &&
		    (pattrib->dhcp_pkt != 1)) {
			/* Non EAP & ARP & DHCP type data packet */

			fill_txdesc_vcs(pattrib, ptxdesc);
			fill_txdesc_phy(pattrib, ptxdesc);

			ptxdesc->rtsrate = 8; /* RTS Rate=24M */
			ptxdesc->data_ratefb_lmt = 0x1F;
			ptxdesc->rts_ratefb_lmt = 0xF;

#if (RATE_ADAPTIVE_SUPPORT == 1)
			/* driver-based RA*/
			if (pattrib->ht_en)
				ptxdesc->sgi = ODM_RA_GetShortGI_8188E(&pHalData->odmpriv, pattrib->mac_id);
			ptxdesc->datarate = ODM_RA_GetDecisionRate_8188E(&pHalData->odmpriv, pattrib->mac_id);

#if (POWER_TRAINING_ACTIVE == 1)
			ptxdesc->pwr_status = ODM_RA_GetHwPwrStatus_8188E(&pHalData->odmpriv, pattrib->mac_id);
#endif
#else /* (RATE_ADAPTIVE_SUPPORT == 1) */
			/* FW-based RA, TODO */
			if (pattrib->ht_en)
				ptxdesc->sgi = 1;

			ptxdesc->datarate = 0x13; /* MCS7 */
#endif /* (RATE_ADAPTIVE_SUPPORT == 1) */

			if (padapter->fix_rate != 0xFF) {
				ptxdesc->userate = 1;
				ptxdesc->datarate = padapter->fix_rate;
				ptxdesc->disdatafb = 1;
				ptxdesc->sgi = (padapter->fix_rate & BIT(7)) ? 1 : 0;
			}
		} else {
			/* EAP data packet and ARP and DHCP packet. */
			/* Use the 1M or 6M data rate to send the EAP/ARP packet. */
			/* This will maybe make the handshake smooth. */

			ptxdesc->bk = 1; /* AGG BK	 */

			if (pmlmeinfo->preamble_mode == PREAMBLE_SHORT)
				ptxdesc->data_short = 1;/* DATA_SHORT */

			ptxdesc->datarate = MRateToHwRate(pmlmeext->tx_rate);
		}

		ptxdesc->usb_txagg_num = pxmitframe->agg_num;
	} else if (pxmitframe->frame_tag == MGNT_FRAMETAG) {

		ptxdesc->macid = pattrib->mac_id; /* CAM_ID(MAC_ID) */
		ptxdesc->qsel = pattrib->qsel;
		ptxdesc->rate_id = pattrib->raid; /* Rate ID */
		ptxdesc->seq = pattrib->seqnum;
		ptxdesc->userate = 1; /* driver uses rate, 1M */
		ptxdesc->rty_lmt_en = 1; /* retry limit enable */
		ptxdesc->data_rt_lmt = 6; /* retry limit = 6 */

#ifdef CONFIG_XMIT_ACK
		/* CCX-TXRPT ack for xmit mgmt frames. */
		if (pxmitframe->ack_report) {
#ifdef DBG_CCX
			static u16 ccx_sw = 0x123;
			txdesc_set_ccx_sw_88e(ptxdesc, ccx_sw);
			RTW_INFO("%s set ccx, sw:0x%03x\n", __func__, ccx_sw);
			ccx_sw = (ccx_sw + 1) % 0xfff;
#endif
			ptxdesc->ccx = 1;
		}
#endif /* CONFIG_XMIT_ACK */

		ptxdesc->datarate = MRateToHwRate(pattrib->rate);

	} else if (pxmitframe->frame_tag == TXAGG_FRAMETAG) {
	}
#ifdef CONFIG_MP_INCLUDED
	else if (pxmitframe->frame_tag == MP_FRAMETAG) {
		struct tx_desc *pdesc;

		pdesc = (struct tx_desc *)ptxdesc;
		fill_txdesc_for_mp(padapter, (u8 *)pdesc);

		pdesc->txdw0 = le32_to_cpu(pdesc->txdw0);
		pdesc->txdw1 = le32_to_cpu(pdesc->txdw1);
		pdesc->txdw2 = le32_to_cpu(pdesc->txdw2);
		pdesc->txdw3 = le32_to_cpu(pdesc->txdw3);
		pdesc->txdw4 = le32_to_cpu(pdesc->txdw4);
		pdesc->txdw5 = le32_to_cpu(pdesc->txdw5);
		pdesc->txdw6 = le32_to_cpu(pdesc->txdw6);
		pdesc->txdw7 = le32_to_cpu(pdesc->txdw7);
	}
#endif /* CONFIG_MP_INCLUDED */
	else {

		ptxdesc->macid = 4; /* CAM_ID(MAC_ID) */
		ptxdesc->rate_id = 6; /* Rate ID */
		ptxdesc->seq = pattrib->seqnum;
		ptxdesc->userate = 1; /* driver uses rate */
		ptxdesc->datarate = MRateToHwRate(pmlmeext->tx_rate);
	}

	ptxdesc->pktlen = pattrib->last_txcmdsz;
	if (pxmitframe->frame_tag == DATA_FRAMETAG) {
#ifdef CONFIG_TX_EARLY_MODE
		ptxdesc->offset = TXDESC_SIZE + EARLY_MODE_INFO_SIZE ;
		ptxdesc->pkt_offset = 0x01;
#else
		ptxdesc->offset = TXDESC_SIZE ;
		ptxdesc->pkt_offset = 0;
#endif
	} else
		ptxdesc->offset = TXDESC_SIZE ;

	if (bmcst)
		ptxdesc->bmc = 1;
	ptxdesc->ls = 1;
	ptxdesc->fs = 1;
	ptxdesc->own = 1;

	/* 2009.11.05. tynli_test. Suggested by SD4 Filen for FW LPS. */
	/* (1) The sequence number of each non-Qos frame / broadcast / multicast / */
	/* mgnt frame should be controled by Hw because Fw will also send null data */
	/* which we cannot control when Fw LPS enable. */
	/* --> default enable non-Qos data sequense number. 2010.06.23. by tynli. */
	/* (2) Enable HW SEQ control for beacon packet, because we use Hw beacon. */
	/* (3) Use HW Qos SEQ to control the seq num of Ext port non-Qos packets. */
	/* 2010.06.23. Added by tynli. */
	if (!pattrib->qos_en) {
		/* Hw set sequence number */
		ptxdesc->hwseq_en = 1; /* HWSEQ_EN */
		ptxdesc->hwseq_sel = 0; /* HWSEQ_SEL */
	}

}
#endif

/*
 *	Description:
 *
 *	Parameters:
 *		pxmitframe	xmitframe
 *		pbuf		where to fill tx desc
 */
void rtl8192es_update_txdesc(struct xmit_frame *pxmitframe, u8 *pbuf)
{
	_rtw_memset(pbuf, 0, TXDESC_SIZE);
	rtl8192es_fill_default_txdesc(pxmitframe, pbuf);
}

static u8 rtw_sdio_wait_enough_TxOQT_space(PADAPTER padapter, u8 agg_num)
{
	u32 n = 0;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);

	while (pHalData->SdioTxOQTFreeSpace < agg_num) {
		if (RTW_CANNOT_RUN(padapter)) {
			RTW_INFO("%s: bSurpriseRemoved or bDriverStopped (wait TxOQT)\n", __func__);
			return _FALSE;
		}

		HalQueryTxOQTBufferStatus8192ESdio(padapter);

		if ((++n % 60) == 0) {
			if ((n % 300) == 0) {
				RTW_INFO("%s(%d): QOT free space(%d), agg_num: %d\n",
					__func__, n, pHalData->SdioTxOQTFreeSpace, agg_num);
			}
			rtw_msleep_os(1);
			/* yield(); */
		}
	}

	pHalData->SdioTxOQTFreeSpace -= agg_num;

	/* if (n > 1) */
	/*	++priv->pshare->nr_out_of_txoqt_space; */

	return _TRUE;
}

/* todo: static */
s32 _dequeue_writeport(PADAPTER padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(padapter);
	struct xmit_buf *pxmitbuf;
	u8	PageIdx = 0;
	u32	deviceId;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	u8	bUpdatePageNum = _FALSE;
#else
	u32	polling_num = 0;
#endif

	pxmitbuf = select_and_dequeue_pending_xmitbuf(padapter);

	if (pxmitbuf == NULL)
		return _TRUE;

	deviceId = ffaddr2deviceId(pdvobjpriv, pxmitbuf->ff_hwaddr);

	/* translate fifo addr to queue index */
	switch (deviceId) {
	case WLAN_TX_HIQ_DEVICE_ID:
		PageIdx = HI_QUEUE_IDX;
		break;

	case WLAN_TX_MIQ_DEVICE_ID:
		PageIdx = MID_QUEUE_IDX;
		break;

	case WLAN_TX_LOQ_DEVICE_ID:
		PageIdx = LOW_QUEUE_IDX;
		break;
	}

query_free_page:
	/* check if hardware tx fifo page is enough */
	if (_FALSE == rtw_hal_sdio_query_tx_freepage(padapter, PageIdx, pxmitbuf->pg_num)) {
		if (RTW_CANNOT_RUN(padapter))
			goto free_xmitbuf;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		if (!bUpdatePageNum) {
			/* Total number of page is NOT available, so update current FIFO status */
			HalQueryTxBufferStatus8192ESdio(padapter);
			bUpdatePageNum = _TRUE;
			goto query_free_page;
		} else {
			bUpdatePageNum = _FALSE;
			enqueue_pending_xmitbuf_to_head(pxmitpriv, pxmitbuf);
			return _TRUE;
		}
#else /* CONFIG_SDIO_TX_ENABLE_AVAL_INT */
		polling_num++;
		if ((polling_num % 60) == 0) {/* or 80 */
			/* RTW_INFO("%s: FIFO starvation!(%d) len=%d agg=%d page=(R)%d(A)%d\n", */
			/*	__func__, n, pxmitbuf->len, pxmitbuf->agg_num, pframe->pg_num, freePage[PageIdx] + freePage[PUBLIC_QUEUE_IDX]); */
			enqueue_pending_xmitbuf_to_head(pxmitpriv, pxmitbuf);
			rtw_msleep_os(1);
			return _FALSE;
		}

		/* Total number of page is NOT available, so update current FIFO status */
		HalQueryTxBufferStatus8192ESdio(padapter);
		goto query_free_page;
#endif /* CONFIG_SDIO_TX_ENABLE_AVAL_INT */
	}

	if (rtw_sdio_wait_enough_TxOQT_space(padapter, pxmitbuf->agg_num) == _FALSE)
		goto free_xmitbuf;

	rtw_write_port(padapter, deviceId, pxmitbuf->len, (u8 *)pxmitbuf);

	rtw_hal_sdio_update_tx_freepage(padapter, PageIdx, pxmitbuf->pg_num);

free_xmitbuf:
	/* rtw_free_xmitframe(pxmitpriv, pframe); */
	/* pxmitbuf->priv_data = NULL; */
	rtw_free_xmitbuf(pxmitpriv, pxmitbuf);

#ifdef CONFIG_SDIO_TX_TASKLET
	tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#endif

	return _FALSE;
}

/*
 * Description
 *	Transmit xmitbuf to hardware tx fifo
 *
 * Return
 *	_SUCCESS	ok
 *	_FAIL		something error
 */
s32 rtl8192es_xmit_buf_handler(PADAPTER padapter)
{
	struct xmit_priv *pxmitpriv;
	u8	queue_empty;
	s32	ret;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);


	pxmitpriv = &padapter->xmitpriv;

	ret = _rtw_down_sema(&pxmitpriv->xmit_sema);
	if (ret == _FAIL) {
		return _FAIL;
	}

	if (RTW_CANNOT_RUN(padapter)) {
		RTW_DBG(FUNC_ADPT_FMT "- bDriverStopped(%s) bSurpriseRemoved(%s)\n",
			FUNC_ADPT_ARG(padapter),
			rtw_is_drv_stopped(padapter) ? "True" : "False",
			rtw_is_surprise_removed(padapter) ? "True" : "False");
		return _FAIL;
	}

	if (rtw_mi_check_pending_xmitbuf(padapter) == 0)
		return _SUCCESS;

#ifdef CONFIG_LPS_LCLK
	ret = rtw_register_tx_alive(padapter);
	if (ret != _SUCCESS)
		return _SUCCESS;
#endif

	do {
		queue_empty = rtw_mi_dequeue_writeport(padapter);

	} while (!queue_empty);

#ifdef CONFIG_LPS_LCLK
	rtw_unregister_tx_alive(padapter);
#endif
	return _SUCCESS;
}

#if 0
/*
 * Description:
 *	Aggregation packets and send to hardware
 *
 * Return:
 *	0	Success
 *	-1	Hardware resource(TX FIFO) not ready
 *	-2	Software resource(xmitbuf) not ready
 */
#ifdef CONFIG_TX_EARLY_MODE
#if RTL8188E_EARLY_MODE_PKT_NUM_10 == 1
	#define EARLY_MODE_MAX_PKT_NUM	10
#else
	#define EARLY_MODE_MAX_PKT_NUM	5
#endif


struct EMInfo {
	u8	EMPktNum;
	u16  EMPktLen[EARLY_MODE_MAX_PKT_NUM];
};


void
InsertEMContent_8188E(
	struct EMInfo *pEMInfo,
	IN pu1Byte	VirtualAddress)
{

#if RTL8188E_EARLY_MODE_PKT_NUM_10 == 1
	u1Byte index = 0;
	u4Byte	dwtmp = 0;
#endif

	_rtw_memset(VirtualAddress, 0, EARLY_MODE_INFO_SIZE);
	if (pEMInfo->EMPktNum == 0)
		return;

#ifdef DBG_EMINFO
	{
		int i;
		RTW_INFO("\n%s ==> pEMInfo->EMPktNum =%d\n", __FUNCTION__, pEMInfo->EMPktNum);
		for (i = 0; i < EARLY_MODE_MAX_PKT_NUM; i++)
			RTW_INFO("%s ==> pEMInfo->EMPktLen[%d] =%d\n", __FUNCTION__, i, pEMInfo->EMPktLen[i]);

	}
#endif

#if RTL8188E_EARLY_MODE_PKT_NUM_10 == 1
	SET_EARLYMODE_PKTNUM(VirtualAddress, pEMInfo->EMPktNum);

	if (pEMInfo->EMPktNum == 1)
		dwtmp = pEMInfo->EMPktLen[0];
	else {
		dwtmp = pEMInfo->EMPktLen[0];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[1];
	}
	SET_EARLYMODE_LEN0(VirtualAddress, dwtmp);
	if (pEMInfo->EMPktNum <= 3)
		dwtmp = pEMInfo->EMPktLen[2];
	else {
		dwtmp = pEMInfo->EMPktLen[2];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[3];
	}
	SET_EARLYMODE_LEN1(VirtualAddress, dwtmp);
	if (pEMInfo->EMPktNum <= 5)
		dwtmp = pEMInfo->EMPktLen[4];
	else {
		dwtmp = pEMInfo->EMPktLen[4];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[5];
	}
	SET_EARLYMODE_LEN2_1(VirtualAddress, dwtmp & 0xF);
	SET_EARLYMODE_LEN2_2(VirtualAddress, dwtmp >> 4);
	if (pEMInfo->EMPktNum <= 7)
		dwtmp = pEMInfo->EMPktLen[6];
	else {
		dwtmp = pEMInfo->EMPktLen[6];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[7];
	}
	SET_EARLYMODE_LEN3(VirtualAddress, dwtmp);
	if (pEMInfo->EMPktNum <= 9)
		dwtmp = pEMInfo->EMPktLen[8];
	else {
		dwtmp = pEMInfo->EMPktLen[8];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[9];
	}
	SET_EARLYMODE_LEN4(VirtualAddress, dwtmp);
#else
	SET_EARLYMODE_PKTNUM(VirtualAddress, pEMInfo->EMPktNum);
	SET_EARLYMODE_LEN0(VirtualAddress, pEMInfo->EMPktLen[0]);
	SET_EARLYMODE_LEN1(VirtualAddress, pEMInfo->EMPktLen[1]);
	SET_EARLYMODE_LEN2_1(VirtualAddress, pEMInfo->EMPktLen[2] & 0xF);
	SET_EARLYMODE_LEN2_2(VirtualAddress, pEMInfo->EMPktLen[2] >> 4);
	SET_EARLYMODE_LEN3(VirtualAddress, pEMInfo->EMPktLen[3]);
	SET_EARLYMODE_LEN4(VirtualAddress, pEMInfo->EMPktLen[4]);
#endif

}



void UpdateEarlyModeInfo8188E(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf)
{
	/* _adapter *padapter, struct xmit_frame *pxmitframe,struct tx_servq	*ptxservq */
	int index, j;
	u16 offset, pktlen;
	PTXDESC_8188E ptxdesc;

	u8 *pmem, *pEMInfo_mem;
	s8 node_num_0 = 0, node_num_1 = 0;
	struct EMInfo eminfo;
	struct agg_pkt_info *paggpkt;
	struct xmit_frame *pframe = (struct xmit_frame *)pxmitbuf->priv_data;
	pmem = pframe->buf_addr;

#ifdef DBG_EMINFO
	RTW_INFO("\n%s ==> agg_num:%d\n", __FUNCTION__, pframe->agg_num);
	for (index = 0; index < pframe->agg_num; index++) {
		offset =	pxmitpriv->agg_pkt[index].offset;
		pktlen = pxmitpriv->agg_pkt[index].pkt_len;
		RTW_INFO("%s ==> agg_pkt[%d].offset=%d\n", __FUNCTION__, index, offset);
		RTW_INFO("%s ==> agg_pkt[%d].pkt_len=%d\n", __FUNCTION__, index, pktlen);
	}
#endif

	if (pframe->agg_num > EARLY_MODE_MAX_PKT_NUM) {
		node_num_0 = pframe->agg_num;
		node_num_1 = EARLY_MODE_MAX_PKT_NUM - 1;
	}

	for (index = 0; index < pframe->agg_num; index++) {
		offset = pxmitpriv->agg_pkt[index].offset;
		pktlen = pxmitpriv->agg_pkt[index].pkt_len;

		_rtw_memset(&eminfo, 0, sizeof(struct EMInfo));
		if (pframe->agg_num > EARLY_MODE_MAX_PKT_NUM) {
			if (node_num_0 > EARLY_MODE_MAX_PKT_NUM) {
				eminfo.EMPktNum = EARLY_MODE_MAX_PKT_NUM;
				node_num_0--;
			} else {
				eminfo.EMPktNum = node_num_1;
				node_num_1--;
			}
		} else
			eminfo.EMPktNum = pframe->agg_num - (index + 1);
		for (j = 0; j < eminfo.EMPktNum ; j++) {
			eminfo.EMPktLen[j] = pxmitpriv->agg_pkt[index + 1 + j].pkt_len + 4; /* CRC */
		}

		if (pmem) {
			ptxdesc = (PTXDESC_8188E)(pmem + offset);
			pEMInfo_mem = pmem + offset + TXDESC_SIZE;
#ifdef DBG_EMINFO
			RTW_INFO("%s ==> desc.pkt_len=%d\n", __FUNCTION__, ptxdesc->pktlen);
#endif
			InsertEMContent_8188E(&eminfo, pEMInfo_mem);
		}


	}
	_rtw_memset(pxmitpriv->agg_pkt, 0, sizeof(struct agg_pkt_info) * MAX_AGG_PKT_NUM);

}
#endif

#endif

#ifdef CONFIG_SDIO_TX_TASKLET
static s32 xmit_xmitframes(PADAPTER padapter, struct xmit_priv *pxmitpriv)
{
	s32 ret;
	_irqL irqL;
	struct xmit_buf	*pxmitbuf;
	struct hw_xmit	*phwxmit = pxmitpriv->hwxmits;
	struct tx_servq	*ptxservq = NULL;
	_list	*xmitframe_plist = NULL, *xmitframe_phead = NULL;
	struct xmit_frame	*pxmitframe = NULL, *pfirstframe = NULL;
	u32	pbuf = 0; /* next pkt address */
	u32	pbuf_tail = 0; /* last pkt tail */
	u32	txlen = 0; /* packet length, except TXDESC_SIZE and PKT_OFFSET */
	u32	total_len = 0, max_xmit_len = 0;
	u8	ac_index = 0;
	u8	bfirst = _TRUE;/* first aggregation xmitframe */
	u8	bulkstart = _FALSE;
#ifdef CONFIG_TX_EARLY_MODE
	u8	pkt_index = 0;
#endif

	pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
	if (pxmitbuf == NULL) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		_rtw_up_sema(&(GET_PRIMARY_ADAPTER(padapter)->xmitpriv.xmit_sema));
#endif
		return _FALSE;
	}

	do {
		/* 3 1. pick up first frame */
		if (bfirst) {
			pxmitframe = rtw_dequeue_xframe(pxmitpriv, pxmitpriv->hwxmits, pxmitpriv->hwxmit_entry);
			if (pxmitframe == NULL) {
				/* no more xmit frame, release xmit buffer */
				rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
				return _FALSE;
			}

			pxmitframe->pxmitbuf = pxmitbuf;
			pxmitframe->buf_addr = pxmitbuf->pbuf;
			pxmitbuf->priv_data = pxmitframe;
			pxmitbuf->ff_hwaddr = rtw_get_ff_hwaddr(pxmitframe);

			pfirstframe = pxmitframe;

			max_xmit_len = rtw_hal_get_sdio_tx_max_length(padapter, pxmitbuf->ff_hwaddr);

			_enter_critical_bh(&pxmitpriv->lock, &irqL);
			ptxservq = rtw_get_sta_pending(padapter, pfirstframe->attrib.psta, pfirstframe->attrib.priority, (u8 *)(&ac_index));
			_exit_critical_bh(&pxmitpriv->lock, &irqL);
		}
		/* 3 2. aggregate same priority and same DA(AP or STA) frames */
		else {
			/* dequeue same priority packet from station tx queue */
			_enter_critical_bh(&pxmitpriv->lock, &irqL);

			if (_rtw_queue_empty(&ptxservq->sta_pending) == _FALSE) {
				xmitframe_phead = get_list_head(&ptxservq->sta_pending);
				xmitframe_plist = get_next(xmitframe_phead);

				pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);

				if (_FAIL == rtw_hal_busagg_qsel_check(padapter, pfirstframe->attrib.qsel, pxmitframe->attrib.qsel))
					bulkstart = _TRUE;
				else {
					/* check xmit_buf size enough or not */
					txlen = TXDESC_SIZE +
#ifdef CONFIG_TX_EARLY_MODE
						EARLY_MODE_INFO_SIZE +
#endif
						rtw_wlan_pkt_size(pxmitframe);

					if ((pbuf + txlen) > max_xmit_len)
						bulkstart = _TRUE;
					else {
						rtw_list_delete(&pxmitframe->list);
						ptxservq->qcnt--;
						phwxmit[ac_index].accnt--;

						/* Remove sta node when there is no pending packets. */
						if (_rtw_queue_empty(&ptxservq->sta_pending) == _TRUE)
							rtw_list_delete(&ptxservq->tx_pending);
					}
				}
			} else
				bulkstart = _TRUE;

			_exit_critical_bh(&pxmitpriv->lock, &irqL);

			if (bulkstart)
				break;

			pxmitframe->buf_addr = pxmitbuf->pbuf + pbuf;

			pxmitframe->agg_num = 0; /* not first frame of aggregation */
		}

		ret = rtw_xmitframe_coalesce(padapter, pxmitframe->pkt, pxmitframe);
		if (ret == _FAIL) {
			RTW_INFO("%s: coalesce FAIL!", __FUNCTION__);
			rtw_free_xmitframe(pxmitpriv, pxmitframe);
			continue;
		}

		/* always return ndis_packet after rtw_xmitframe_coalesce */
		/* rtw_os_xmit_complete(padapter, pxmitframe); */

#ifdef CONFIG_TX_EARLY_MODE
		pxmitpriv->agg_pkt[pkt_index].pkt_len = pxmitframe->attrib.last_txcmdsz;	/* get from rtw_xmitframe_coalesce */
		pxmitpriv->agg_pkt[pkt_index].offset = _RND8(pxmitframe->attrib.last_txcmdsz + TXDESC_SIZE + EARLY_MODE_INFO_SIZE);
		pkt_index++;
#endif

		if (bfirst) {
			txlen = TXDESC_SIZE +
#ifdef CONFIG_TX_EARLY_MODE
				EARLY_MODE_INFO_SIZE +
#endif
				pxmitframe->attrib.last_txcmdsz;

			total_len = txlen;

			pxmitframe->pg_num = PageNum(txlen, PAGE_SIZE_TX_92E);
			pxmitbuf->pg_num = PageNum(txlen, PAGE_SIZE_TX_92E);
			pbuf_tail = txlen;
			pbuf = _RND8(pbuf_tail);
			bfirst = _FALSE;
		} else {
			rtl8192es_update_txdesc(pxmitframe, pxmitframe->buf_addr);

			/* don't need xmitframe any more */
			rtw_free_xmitframe(pxmitpriv, pxmitframe);

			pxmitframe->pg_num = PageNum(txlen, PAGE_SIZE_TX_92E);
			/* pfirstframe->pg_num += pxmitframe->pg_num; */
			pxmitbuf->pg_num += PageNum(txlen, PAGE_SIZE_TX_92E);

			total_len += txlen;

			/* handle pointer and stop condition */
			pbuf_tail = pbuf + txlen;
			pbuf = _RND8(pbuf_tail);

			pfirstframe->agg_num++;
			if (pfirstframe->agg_num >= (rtw_hal_sdio_max_txoqt_free_space(padapter) - 1))
				break;

		}
	} while (1);

	/* 3 3. update first frame txdesc */
	rtl8192es_update_txdesc(pfirstframe, pfirstframe->buf_addr);
#ifdef CONFIG_TX_EARLY_MODE
	UpdateEarlyModeInfo8188E(pxmitpriv, pxmitbuf);
#endif

	/*  */
	pxmitbuf->agg_num = pfirstframe->agg_num;
	pxmitbuf->priv_data = NULL;

	/* 3 4. write xmit buffer to USB FIFO */
	pxmitbuf->len = pbuf_tail;
	enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);

	/* 3 5. update statisitc */
	rtw_count_tx_stats(padapter, pfirstframe, total_len);

	rtw_free_xmitframe(pxmitpriv, pfirstframe);

	/* rtw_yield_os(); */

	return _TRUE;
}

void rtl8192es_xmit_tasklet(void *priv)
{
	int ret = _FALSE;
	_adapter *padapter = (_adapter *)priv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	while (1) {
		if (RTW_CANNOT_TX(padapter)) {
			RTW_INFO("xmit_tasklet => bDriverStopped or bSurpriseRemoved or bWritePortCancel\n");
			break;
		}

		ret = xmit_xmitframes(padapter, pxmitpriv);
		if (ret == _FALSE)
			break;

	}
}
#else
static s32 xmit_xmitframes(PADAPTER padapter, struct xmit_priv *pxmitpriv)
{
	s32 err, agg_num = 0;
	u8 pkt_index = 0;
	struct hw_xmit *hwxmits, *phwxmit;
	u8 idx, hwentry;
	_irqL irql;
	struct tx_servq	*ptxservq;
	_list *sta_plist, *sta_phead, *frame_plist, *frame_phead;
	struct xmit_frame *pxmitframe;
	_queue *pframe_queue;
	struct xmit_buf *pxmitbuf;
	u32 txlen, max_xmit_len;
	s32 ret;
	int inx[4];
	u8 pre_qsel = 0xFF, next_qsel = 0xFF;
	u8 single_sta_in_queue = _FALSE;

	err = 0;
	hwxmits = pxmitpriv->hwxmits;
	hwentry = pxmitpriv->hwxmit_entry;
	ptxservq = NULL;
	pxmitframe = NULL;
	pframe_queue = NULL;
	pxmitbuf = NULL;

	if (padapter->registrypriv.wifi_spec == 1) {
		for (idx = 0; idx < 4; idx++)
			inx[idx] = pxmitpriv->wmm_para_seq[idx];
	} else {
		inx[0] = 0;
		inx[1] = 1;
		inx[2] = 2;
		inx[3] = 3;
	}

	/* 0(VO), 1(VI), 2(BE), 3(BK) */
	for (idx = 0; idx < hwentry; idx++) {
		phwxmit = hwxmits + inx[idx];

		if ((check_pending_xmitbuf(pxmitpriv) == _TRUE) && (padapter->mlmepriv.LinkDetectInfo.bHigherBusyTxTraffic == _TRUE)) {
			if ((phwxmit->accnt > 0) && (phwxmit->accnt < 5)) {
				err = -2;
				break;
			}
		}

		max_xmit_len = rtw_hal_get_sdio_tx_max_length(padapter, inx[idx]);

		/*		_enter_critical(&hwxmits->sta_queue->lock, &irqL0); */
		_enter_critical_bh(&pxmitpriv->lock, &irql);

		sta_phead = get_list_head(phwxmit->sta_queue);
		sta_plist = get_next(sta_phead);

		single_sta_in_queue = rtw_end_of_queue_search(sta_phead, get_next(sta_plist));

		while (rtw_end_of_queue_search(sta_phead, sta_plist) == _FALSE) {
			ptxservq = LIST_CONTAINOR(sta_plist, struct tx_servq, tx_pending);

			sta_plist = get_next(sta_plist);

			pframe_queue = &ptxservq->sta_pending;

			/*			_enter_critical(&pframe_queue->lock, &irqL1); */
			/* _enter_critical_bh(&pxmitpriv->lock, &irql); */

			frame_phead = get_list_head(pframe_queue);

			while (rtw_is_list_empty(frame_phead) == _FALSE) {
				frame_plist = get_next(frame_phead);
				pxmitframe = LIST_CONTAINOR(frame_plist, struct xmit_frame, list);

				/* check xmit_buf size enough or not */
#ifdef CONFIG_TX_EARLY_MODE
				txlen = TXDESC_SIZE + EARLY_MODE_INFO_SIZE + rtw_wlan_pkt_size(pxmitframe);
#else
				txlen = TXDESC_SIZE + rtw_wlan_pkt_size(pxmitframe);
#endif

				next_qsel = pxmitframe->attrib.qsel;

				if ((NULL == pxmitbuf) ||
				    ((_RND(pxmitbuf->len, 8) + txlen) > max_xmit_len)
				    || (agg_num >= (rtw_hal_sdio_max_txoqt_free_space(padapter) - 1))
				    || ((agg_num != 0) && (_FAIL == rtw_hal_busagg_qsel_check(padapter, pre_qsel, next_qsel)))
				   ) {
					if (pxmitbuf) {
						/* pxmitbuf->priv_data will be NULL, and will crash here */
						if (pxmitbuf->len > 0 && pxmitbuf->priv_data) {
							struct xmit_frame *pframe;
							pframe = (struct xmit_frame *)pxmitbuf->priv_data;
							pframe->agg_num = agg_num;
							pxmitbuf->agg_num = agg_num;
							/* RTW_INFO("==> agg_num:%d\n",agg_num); */
							rtl8192es_update_txdesc(pframe, pframe->buf_addr);
#ifdef CONFIG_TX_EARLY_MODE
							UpdateEarlyModeInfo8188E(pxmitpriv, pxmitbuf);
#endif
							rtw_free_xmitframe(pxmitpriv, pframe);
							pxmitbuf->priv_data = NULL;
							enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);

							/* rtw_yield_os(); */
							if (single_sta_in_queue == _FALSE) {
								/* break the loop in case there is more than one sta in this ac queue */
								pxmitbuf = NULL;
								err = -3;
								break;
							}

						} else
							rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
					}

					pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
					if (pxmitbuf == NULL) {
						err = -2;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
						_rtw_up_sema(&(GET_PRIMARY_ADAPTER(padapter)->xmitpriv.xmit_sema));
#endif
						break;
					}
					agg_num = 0;
					pkt_index = 0;
				}

				/* ok to send, remove frame from queue */
				rtw_list_delete(&pxmitframe->list);
				ptxservq->qcnt--;
				phwxmit->accnt--;

				if (agg_num == 0) {
					pxmitbuf->ff_hwaddr = rtw_get_ff_hwaddr(pxmitframe);
					pxmitbuf->priv_data = (u8 *)pxmitframe;
				}

				/* coalesce the xmitframe to xmitbuf */
				pxmitframe->pxmitbuf = pxmitbuf;
				pxmitframe->buf_addr = pxmitbuf->ptail;

				ret = rtw_xmitframe_coalesce(padapter, pxmitframe->pkt, pxmitframe);
				if (ret == _FAIL) {
					RTW_INFO("%s: coalesce FAIL!", __FUNCTION__);
					/* Todo: error handler */
					/* rtw_free_xmitframe(pxmitpriv, pxmitframe); */
				} else {
					agg_num++;
					if (agg_num != 1)
						rtl8192es_update_txdesc(pxmitframe, pxmitframe->buf_addr);
					pre_qsel = pxmitframe->attrib.qsel;
					rtw_count_tx_stats(padapter, pxmitframe, pxmitframe->attrib.last_txcmdsz);
#ifdef CONFIG_TX_EARLY_MODE
					txlen = TXDESC_SIZE + EARLY_MODE_INFO_SIZE + pxmitframe->attrib.last_txcmdsz;
#else
					txlen = TXDESC_SIZE + pxmitframe->attrib.last_txcmdsz;
#endif
					pxmitframe->pg_num = PageNum(txlen, PAGE_SIZE_TX_92E);
					pxmitbuf->pg_num += PageNum(txlen, PAGE_SIZE_TX_92E);
					/* if (agg_num != 1) */
					/* ((struct xmit_frame*)pxmitbuf->priv_data)->pg_num += pxmitframe->pg_num; */

#ifdef CONFIG_TX_EARLY_MODE
					pxmitpriv->agg_pkt[pkt_index].pkt_len = pxmitframe->attrib.last_txcmdsz;	/* get from rtw_xmitframe_coalesce	 */
					pxmitpriv->agg_pkt[pkt_index].offset = _RND8(pxmitframe->attrib.last_txcmdsz + TXDESC_SIZE + EARLY_MODE_INFO_SIZE);
#endif

					pkt_index++;
					pxmitbuf->ptail += _RND(txlen, 8); /* round to 8 bytes alignment */
					pxmitbuf->len = _RND(pxmitbuf->len, 8) + txlen;
				}

				if (agg_num != 1)
					rtw_free_xmitframe(pxmitpriv, pxmitframe);
				pxmitframe = NULL;
			}

			if (_rtw_queue_empty(pframe_queue) == _TRUE)
				rtw_list_delete(&ptxservq->tx_pending);
			else if (err == -3) {
				/* Re-arrange the order of stations in this ac queue to balance the service for these stations */
				rtw_list_delete(&ptxservq->tx_pending);
				rtw_list_insert_tail(&ptxservq->tx_pending, get_list_head(phwxmit->sta_queue));
			}

			if (err)
				break;
		}
		_exit_critical_bh(&pxmitpriv->lock, &irql);

		/* dump xmit_buf to hw tx fifo */
		if (pxmitbuf) {

			if (pxmitbuf->len > 0) {
				struct xmit_frame *pframe;
				pframe = (struct xmit_frame *)pxmitbuf->priv_data;
				pframe->agg_num = agg_num;
				pxmitbuf->agg_num = agg_num;
				rtl8192es_update_txdesc(pframe, pframe->buf_addr);
#ifdef CONFIG_TX_EARLY_MODE
				UpdateEarlyModeInfo8188E(pxmitpriv, pxmitbuf);
#endif
				rtw_free_xmitframe(pxmitpriv, pframe);
				pxmitbuf->priv_data = NULL;
				enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);
				rtw_yield_os();
			} else
				rtw_free_xmitbuf(pxmitpriv, pxmitbuf);

			pxmitbuf = NULL;
		}

		if (err == -2)
			break;
	}

	return err;

}

/*
 * Description
 *	Transmit xmitframe from queue
 *
 * Return
 *	_SUCCESS	ok
 *	_FAIL		something error
 */
s32 rtl8192es_xmit_handler(PADAPTER padapter)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv ;
	s32 ret;
	_irqL irql;

wait:
	ret = _rtw_down_sema(&pxmitpriv->SdioXmitSema);
	if (_FAIL == ret) {
		return _FAIL;
	}

next:

	if (RTW_CANNOT_RUN(padapter)) {
		RTW_DBG(FUNC_ADPT_FMT "- bDriverStopped(%s) bSurpriseRemoved(%s)\n",
			FUNC_ADPT_ARG(padapter),
			rtw_is_drv_stopped(padapter) ? "True" : "False",
			rtw_is_surprise_removed(padapter) ? "True" : "False");
		return _FAIL;
	}
	_enter_critical_bh(&pxmitpriv->lock, &irql);
	ret = rtw_txframes_pending(padapter);
	_exit_critical_bh(&pxmitpriv->lock, &irql);
	if (ret == 0)
		return _SUCCESS;
	/* dequeue frame and write to hardware */

	ret = xmit_xmitframes(padapter, pxmitpriv);
	if (ret == -2) {
		/* rtw_yield_os(); */
		rtw_msleep_os(1);
		goto next;
	}
	_enter_critical_bh(&pxmitpriv->lock, &irql);
	ret = rtw_txframes_pending(padapter);
	_exit_critical_bh(&pxmitpriv->lock, &irql);
	if (ret == 1)
		goto next;

	return _SUCCESS;
}

thread_return rtl8192es_xmit_thread(thread_context context)
{
	s32 ret;
	PADAPTER padapter = (PADAPTER)context;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	ret = _SUCCESS;

	thread_enter("RTWHALXT");

	RTW_INFO("start %s\n", __FUNCTION__);

	do {
		ret = rtl8192es_xmit_handler(padapter);
		flush_signals_thread();
	} while (_SUCCESS == ret);

	RTW_INFO(FUNC_ADPT_FMT " Exit\n", FUNC_ADPT_ARG(padapter));

	rtw_thread_wait_stop();

	return 0;
}
#endif

s32 rtl8192es_mgnt_xmit(PADAPTER padapter, struct xmit_frame *pmgntframe)
{
	s32 ret = _SUCCESS;
	struct pkt_attrib	*pattrib;
	struct xmit_buf	*pxmitbuf;
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(padapter);
	u8	*pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	u8	 pattrib_subtype;


	pattrib = &pmgntframe->attrib;
	pxmitbuf = pmgntframe->pxmitbuf;

	rtl8192es_update_txdesc(pmgntframe, pmgntframe->buf_addr);

	pxmitbuf->len = TXDESC_SIZE + pattrib->last_txcmdsz;

	pxmitbuf->pg_num = PageNum(pxmitbuf->len, PAGE_SIZE_TX_92E);
	pxmitbuf->ptail = pmgntframe->buf_addr + pxmitbuf->len;
	pxmitbuf->ff_hwaddr = rtw_get_ff_hwaddr(pmgntframe);

	rtw_count_tx_stats(padapter, pmgntframe, pattrib->last_txcmdsz);
	pattrib_subtype = pattrib->subtype;
	rtw_free_xmitframe(pxmitpriv, pmgntframe);

	pxmitbuf->priv_data = NULL;

	if ((pattrib_subtype == WIFI_BEACON) || (get_frame_sub_type(pframe) == WIFI_BEACON)) { /* dump beacon directly */
		ret = rtw_write_port(padapter, ffaddr2deviceId(pdvobjpriv, pxmitbuf->ff_hwaddr), pxmitbuf->len, (u8 *)pxmitbuf);
		if (ret != _SUCCESS)
			rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_WRITE_PORT_ERR);

		rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
	} else
		enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);

	return ret;
}

/*
 * Description:
 *	Handle xmitframe(packet) come from rtw_xmit()
 *
 * Return:
 *	_TRUE	dump packet directly ok
 *	_FALSE	enqueue, temporary can't transmit packets to hardware
 */
s32 rtl8192es_hal_xmit(PADAPTER padapter, struct xmit_frame *pxmitframe)
{
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	_irqL irql;
	s32 err;

	/* pxmitframe->attrib.qsel = pxmitframe->attrib.priority; */

#ifdef CONFIG_80211N_HT
	if ((pxmitframe->frame_tag == DATA_FRAMETAG) &&
	    (pxmitframe->attrib.ether_type != 0x0806) &&
	    (pxmitframe->attrib.ether_type != 0x888e) &&
	    (pxmitframe->attrib.dhcp_pkt != 1)) {
		if (padapter->mlmepriv.LinkDetectInfo.bBusyTraffic == _TRUE)
			rtw_issue_addbareq_cmd(padapter, pxmitframe);
	}
#endif

	_enter_critical_bh(&pxmitpriv->lock, &irql);
	err = rtw_xmitframe_enqueue(padapter, pxmitframe);
	_exit_critical_bh(&pxmitpriv->lock, &irql);
	if (err != _SUCCESS) {
		rtw_free_xmitframe(pxmitpriv, pxmitframe);

		pxmitpriv->tx_drop++;
		return _TRUE;
	}

#ifdef CONFIG_SDIO_TX_TASKLET
	tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#else
	_rtw_up_sema(&pxmitpriv->SdioXmitSema);
#endif

	return _FALSE;
}

s32	rtl8192es_hal_xmitframe_enqueue(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	s32 err;

	err = rtw_xmitframe_enqueue(padapter, pxmitframe);
	if (err != _SUCCESS) {
		rtw_free_xmitframe(pxmitpriv, pxmitframe);

		pxmitpriv->tx_drop++;
	} else {
#ifdef CONFIG_SDIO_TX_TASKLET
		tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#else
		_rtw_up_sema(&pxmitpriv->SdioXmitSema);
#endif
	}

	return err;

}


/*
 * Return
 *	_SUCCESS	start thread ok
 *	_FAIL		start thread fail
 *
 */
s32 rtl8192es_init_xmit_priv(PADAPTER padapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

#ifdef CONFIG_SDIO_TX_TASKLET
#ifdef PLATFORM_LINUX
	tasklet_init(&pxmitpriv->xmit_tasklet,
		     (void(*)(unsigned long))rtl8192es_xmit_tasklet,
		     (unsigned long)padapter);
#endif
#else /* CONFIG_SDIO_TX_TASKLET */

	_rtw_init_sema(&pxmitpriv->SdioXmitSema, 0);

#endif /* CONFIG_SDIO_TX_TASKLET */

	_rtw_spinlock_init(&pHalData->SdioTxFIFOFreePageLock);
	rtl8192e_init_xmit_priv(padapter);

	return _SUCCESS;
}

void rtl8192es_free_xmit_priv(PADAPTER padapter)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);

	_rtw_spinlock_free(&pHalData->SdioTxFIFOFreePageLock);
}
